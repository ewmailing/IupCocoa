/** \file
 * \brief Tree Control
 *
 * See Copyright Notice in iup.h
 */

#import <Cocoa/Cocoa.h>
#import <objc/runtime.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_layout.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_stdcontrols.h"
#include "iup_key.h"
#include "iup_image.h"
#include "iup_array.h"
#include "iup_tree.h"

#include "iup_drvinfo.h"

#include "iupcocoa_drv.h"

// TODO:
// Removing the disclosure triangle, Disable indenting
// https://stackoverflow.com/questions/4251790/nsoutlineview-remove-disclosure-triangle-and-indent

// the point of this is we have a unique memory address for an identifier
static const void* IUP_COCOA_TREE_DELEGATE_OBJ_KEY = "IUP_COCOA_TREE_DELEGATE_OBJ_KEY";

static  NSString* const IUPCOCOA_OUTLINEVIEW_DRAGANDDROP_TYPE = @"iupcocoa.outlineview.dragdrop";



static NSView* cocoaTreeGetRootView(Ihandle* ih)
{
	NSView* root_container_view = (NSView*)ih->handle;
	NSCAssert([root_container_view isKindOfClass:[NSView class]], @"Expected NSView");
	return root_container_view;
}

static NSScrollView* cocoaTreeGetScrollView(Ihandle* ih)
{
	NSScrollView* scroll_view = (NSScrollView*)ih->handle;
	NSCAssert([scroll_view isKindOfClass:[NSScrollView class]], @"Expected NSScrollView");
	return scroll_view;
}

static NSOutlineView* cocoaTreeGetOutlineView(Ihandle* ih)
{
	
	NSScrollView* scroll_view = cocoaTreeGetScrollView(ih);
	NSOutlineView* outline_view = (NSOutlineView*)[scroll_view documentView];
	NSCAssert([outline_view isKindOfClass:[NSOutlineView class]], @"Expected NSOutlineView");
	return outline_view;
	
}


@interface IupCocoaTreeItem : NSObject
{
	IupCocoaTreeItem* parentItem;
	NSMutableArray* childrenArray;
	int kind; // ITREE_BRANCH ITREE_LEAF
	NSString* title;
	BOOL isDeleted;
	NSImage* bitmapImage;
	NSImage* collapsedImage;
	NSTableCellView* tableCellView; // kind of a hack to force layout in heightOf
	
}

@property(nonatomic, assign) int kind;
@property(nonatomic, copy) NSString* title;
@property(nonatomic, weak) IupCocoaTreeItem* parentItem;
@property(nonatomic, assign) BOOL isDeleted;
@property(nonatomic, retain) NSImage* bitmapImage;
@property(nonatomic, retain) NSImage* collapsedImage;
@property(nonatomic, weak) NSTableCellView* tableCellView; // this is kind of a hack to force layout in heightOf. I'm not sure if I want to keep a strong reference because I don't know if there is a possible circular reference here.

- (instancetype) clone;

- (IupCocoaTreeItem*) childAtIndex:(NSUInteger)the_index;

@end

// forward declaration needed
static void cocoaTreeReloadItem(IupCocoaTreeItem* tree_item, NSOutlineView* outline_view);


@implementation IupCocoaTreeItem

@synthesize kind = kind;
@synthesize title = title;
@synthesize parentItem = parentItem;
@synthesize isDeleted = isDeleted;
@synthesize bitmapImage = bitmapImage; // is the expandedImage for branches
@synthesize collapsedImage = collapsedImage;
@synthesize tableCellView = tableCellView;


// Creates, caches, and returns the array of children
// Loads children incrementally
- (NSMutableArray*) childrenArray
{
	return childrenArray;
}

- (void) setChildrenArray:(NSMutableArray*)new_array
{
	if(childrenArray == new_array)
	{
		return;
	}
	[childrenArray release];
	childrenArray = [new_array retain];
}


- (IupCocoaTreeItem*) childAtIndex:(NSUInteger)the_index
{
	return [[self childrenArray] objectAtIndex:the_index];
}


- (NSUInteger) numberOfChildren
{
	NSArray* tmp = [self childrenArray];
	return [tmp count];
}


- (instancetype) init
{
	self = [super init];
	if(self)
	{
		childrenArray = [[NSMutableArray alloc] init];
	}
	return self;
}

- (void) dealloc
{
	//[tableCellView releae];
	tableCellView = nil; // weak ref
	
	[bitmapImage release];
	[collapsedImage release];

	[childrenArray release];
	[title release];
	parentItem = nil; // weak ref
	[super dealloc];
}

- (instancetype) cloneWithNewParentItem:(IupCocoaTreeItem*)new_parent_item
{
	IupCocoaTreeItem* new_copy = [[IupCocoaTreeItem alloc] init];
	[new_copy setParentItem:new_parent_item];
	[new_copy setTableCellView:nil];
	[new_copy setIsDeleted:NO];
	
	[new_copy setBitmapImage:[self bitmapImage]];
	[new_copy setCollapsedImage:[self collapsedImage]];

	[new_copy setTitle:[self title]]; // this is a copy property
	[new_copy setKind:[self kind]];

	NSMutableArray* new_children_array = [[NSMutableArray alloc] init];
	[new_copy setChildrenArray:new_children_array];
	[new_children_array autorelease];
	
	for(IupCocoaTreeItem* original_item in childrenArray)
	{
		IupCocoaTreeItem* child_copy = [original_item cloneWithNewParentItem:new_copy];
		[new_children_array addObject:child_copy];
	}

	return new_copy;
}

@end
/*
@interface IupCocoaTreeRoot : NSObject
{
	// Array of IupCocoaTreeItems
	NSMutableArray* topLevelObjects;
}
@end

@implementation IupCocoaTreeRoot

@end
*/

// We need to override NSOutlineView in order to implement things like keyDown for k_any
@interface IupCocoaOutlineView : NSOutlineView
{
	Ihandle* _ih;
	NSImage* leafImage;
	NSImage* expandedImage;
	NSImage* collapsedImage;
}
@property(nonatomic, assign) Ihandle* ih;
@property(nonatomic, retain) NSImage* leafImage;
@property(nonatomic, retain) NSImage* expandedImage;
@property(nonatomic, retain) NSImage* collapsedImage;
@end

@implementation IupCocoaOutlineView
@synthesize ih = _ih;
@synthesize leafImage = leafImage;
@synthesize expandedImage = expandedImage;
@synthesize collapsedImage = collapsedImage;

- (void) dealloc
{
	[leafImage release];
	[expandedImage release];
	[collapsedImage release];
	[super dealloc];
}

// TODO: k_any
- (void) keyDown:(NSEvent*)the_event
{
	NSLog(@"keyDown: %@", the_event);
	[super keyDown:the_event];
	
}

@end

// We are not using NSComboBoxDataSource
@interface IupCocoaTreeDelegate : NSObject <NSOutlineViewDataSource, NSOutlineViewDelegate>
{
	NSMutableArray* treeRootTopLevelObjects;
	
//	NSMutableArray* orderedArrayOfSelections; // TODO: If we decide selection order is important enough and worth the risks of edge cases missing updates (like delnode)
	NSIndexSet* previousSelections;
	
	id itemBeingDragged; // This is used to compare whether the item being dragged ends up dragging onto itself so we can reject it in validation
}
@property(nonatomic, copy) NSArray* treeRootTopLevelObjects; // This is intended for external read-only access to iterate through all items, such as changing the branch/leaf images
@property(nonatomic, weak) id itemBeingDragged;
- (NSUInteger) insertChild:(IupCocoaTreeItem*)tree_item_child withParent:(IupCocoaTreeItem*)tree_item_parent;
- (NSUInteger) insertChild:(IupCocoaTreeItem*)tree_item_child withParent:(IupCocoaTreeItem*)tree_item_parent targetChildIndex:(NSInteger)target_child_index;
- (NSUInteger) insertPeer:(IupCocoaTreeItem*)tree_item_new withSibling:(IupCocoaTreeItem*)tree_item_prev;
- (void) insertAtRoot:(IupCocoaTreeItem*)tree_item_new;
- (void) removeAllObjects;
- (NSIndexSet*) removeAllChildrenForItem:(IupCocoaTreeItem*)tree_item;
- (NSUInteger) removeItem:(IupCocoaTreeItem*)tree_item;
- (void) moveItem:(IupCocoaTreeItem*)tree_item targetParent:(IupCocoaTreeItem*)tree_item_parent targetChildIndex:(NSInteger)target_child_index;

//- (NSMutableArray*) dataArray;

// NSOutlineViewDataSource
- (NSInteger) outlineView:(NSOutlineView*)outline_view numberOfChildrenOfItem:(nullable id)the_item;
//- (id) outlineView:(NSOutlineView*)outline_view child:(NSInteger)index ofItem:(nullable id)the_item;
- (BOOL) outlineView:(NSOutlineView*)outline_view isItemExpandable:(id)the_item;
// NSOutlineViewDelegate
- (nullable NSView *)outlineView:(NSOutlineView*)outline_view viewForTableColumn:(nullable NSTableColumn*)table_column item:(id)the_item;
// NSOutlineViewDelegate
- (void) outlineViewSelectionDidChange:(NSNotification*)the_notification;
// NSOutlineViewDelegate, for CANFOCUS
- (NSIndexSet*) outlineView:(NSOutlineView*)outline_view selectionIndexesForProposedSelection:(NSIndexSet*)proposed_selection_indexes;

@end

static NSUInteger Helper_RecursivelyCountItems(IupCocoaTreeItem* the_item)
{
	NSUInteger counter = 1;
	for(IupCocoaTreeItem* a_item in [the_item childrenArray])
	{
		counter += Helper_RecursivelyCountItems(a_item);
	}
	return counter;
}

@implementation IupCocoaTreeDelegate
@synthesize treeRootTopLevelObjects = treeRootTopLevelObjects;
@synthesize itemBeingDragged = itemBeingDragged;

- (instancetype) init
{
	self = [super init];
	if(self)
	{
		treeRootTopLevelObjects = [[NSMutableArray alloc] init];
	}
	return self;
}

- (void) dealloc
{
	[treeRootTopLevelObjects release];
	[previousSelections release];
	[super dealloc];
}

- (NSUInteger) insertChild:(IupCocoaTreeItem*)tree_item_child withParent:(IupCocoaTreeItem*)tree_item_parent
{
	[self insertChild:tree_item_child withParent:tree_item_parent targetChildIndex:0];
	return 0; // always index 0 since we always insert in the first position
}

- (NSUInteger) insertChild:(IupCocoaTreeItem*)tree_item_child withParent:(IupCocoaTreeItem*)tree_item_parent targetChildIndex:(NSInteger)target_child_index
{
	// IUP always inserts the child in the first position, not the last, when in this parent/child relationship
	[[tree_item_parent childrenArray] insertObject:tree_item_child atIndex:target_child_index];
	[tree_item_child setParentItem:tree_item_parent];
	return target_child_index;
}

- (NSUInteger) insertPeer:(IupCocoaTreeItem*)tree_item_new withSibling:(IupCocoaTreeItem*)tree_item_prev
{
	IupCocoaTreeItem* tree_item_parent = [tree_item_prev parentItem];
	if(nil != tree_item_parent)
	{
		[tree_item_new setParentItem:tree_item_parent];
		// insert the new node after reference node
		NSMutableArray* children_array = [tree_item_parent childrenArray];
		NSUInteger prev_index = [children_array indexOfObject:tree_item_prev];
		NSUInteger target_index = prev_index + 1;

		if(target_index > [children_array count])
		{
			target_index = [children_array count];
			[children_array addObject:tree_item_new];
		}
		else
		{
			[children_array insertObject:tree_item_new atIndex:target_index];
		}
		return target_index;
	}
	else
	{
		// we are adding a peer to the root (case is ADDROOT=NO)
		NSUInteger target_index = 0;

		NSUInteger object_index = [treeRootTopLevelObjects indexOfObject:tree_item_prev];
		if(object_index != NSNotFound)
		{
			// insert after the previous (reference) node
			target_index = object_index + 1;
		}
	
		[treeRootTopLevelObjects insertObject:tree_item_new atIndex:target_index];
		return target_index;
	}
}

- (void) insertAtRoot:(IupCocoaTreeItem*)tree_item_new
{
	// IUP always inserts the child in the first position, not the last
	[treeRootTopLevelObjects insertObject:tree_item_new atIndex:0];
}

- (void) removeAllObjects
{
	[treeRootTopLevelObjects removeAllObjects];
}

// Returns the indexes of the top-level children that get removed
- (NSIndexSet*) removeAllChildrenForItem:(IupCocoaTreeItem*)tree_item
{
	if(nil == tree_item)
	{
		return nil;
	}
	NSUInteger number_of_descendents = Helper_RecursivelyCountItems(tree_item) - 1; // subtract one because we don't want to count the tree_item itself, just children/grandchildren

	NSMutableArray* children_array = [tree_item childrenArray];
	NSIndexSet* top_level_children_indexes = [NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, [children_array count])];
	[children_array removeAllObjects];
	return top_level_children_indexes;
}

// This is a helper for removeItem:
// This is a special helper because when using fast enumeration in removeItem: we can't change the parent's childrenArray to remove this node.
// So in this helper, we don't try and assume removeItem: will handle that last step itself.
// So this method is here so we can clear the children data and update the count.
- (void) removeRecursiveChildItemHelper:(IupCocoaTreeItem*)tree_item
{
	// First, if this node has any children, recursively traverse through all the children and remove them.
	NSMutableArray* children_array = [tree_item childrenArray];
	for(IupCocoaTreeItem* an_item in children_array)
	{
		[self removeRecursiveChildItemHelper:an_item];
	}
	// clear the children array so in case there is another reference that is still using this pointer, it will have updated info that there are no children.
	[children_array removeAllObjects];
	[tree_item setIsDeleted:YES];
}

- (NSUInteger) removeItem:(IupCocoaTreeItem*)tree_item
{
	if(nil == tree_item)
	{
		return NSNotFound;
	}
	// If we already removed this item, the parentItem is nil.
	if(YES == [tree_item isDeleted])
	{
		return NSNotFound;
	}
	
	// First, if this node has any children, recursively traverse through all the children and remove them.
	NSMutableArray* children_array = [tree_item childrenArray];
	for(IupCocoaTreeItem* an_item in children_array)
	{
		[self removeRecursiveChildItemHelper:an_item];
	}
	// clear the children array so in case there is another reference that is still using this pointer, it will have updated info that there are no children.
	[children_array removeAllObjects];

	// now remove this node by going to the parent and removing this from the parent's childrenArray
	IupCocoaTreeItem* tree_item_parent = [tree_item parentItem];
	if(nil != tree_item_parent)
	{
		NSUInteger object_index = [[tree_item_parent childrenArray] indexOfObject:tree_item];
		if(object_index != NSNotFound)
		{
			[[tree_item_parent childrenArray] removeObjectAtIndex:object_index];
		}
		return object_index;
	}
	else
	{
		// this is top level node
		NSUInteger object_index = [treeRootTopLevelObjects indexOfObject:tree_item];
		if(object_index != NSNotFound)
		{
			[treeRootTopLevelObjects removeObjectAtIndex:object_index];
		}
		return object_index;
	}
}

- (void) moveItem:(IupCocoaTreeItem*)tree_item targetParent:(IupCocoaTreeItem*)target_parent_tree_item targetChildIndex:(NSInteger)target_child_index
{
	IupCocoaTreeItem* parent_tree_item = [tree_item parentItem];

	// remove from the old location (don't call removeItem: because it kills the children)
	if(parent_tree_item)
	{
		[[parent_tree_item childrenArray] removeObject:tree_item];
	}
	else
	{
		[treeRootTopLevelObjects removeObject:tree_item];
	}
	
	// insert to the new location
	if(target_parent_tree_item)
	{
		[[target_parent_tree_item childrenArray] insertObject:tree_item atIndex:target_child_index];
	}
	else
	{
		[treeRootTopLevelObjects insertObject:tree_item atIndex:target_child_index];
	}

	// Now that the node is moved, it has a new parent
	[tree_item setParentItem:target_parent_tree_item];
}

- (NSInteger) outlineView:(NSOutlineView*)outline_view numberOfChildrenOfItem:(nullable id)the_item
{
	// FIXME: temp placeholder
	// FIXME: temp placeholder
	if(nil == the_item)
	{
		NSInteger the_count = [treeRootTopLevelObjects count];
		return the_count;
	}
	else
	{
		NSInteger the_count = [the_item numberOfChildren];
		return the_count;
	}
}

- (id) outlineView:(NSOutlineView*)outline_view child:(NSInteger)the_index ofItem:(nullable id)the_item
{
	// FIXME: temp placeholder
	if(nil == the_item)
	{
//		return nil;
//		IupCocoaTreeItem* dummy = [[[IupCocoaTreeItem alloc] init] autorelease];
// return dummy;
		IupCocoaTreeItem* tree_item = [treeRootTopLevelObjects objectAtIndex:the_index];
		return tree_item;
	}
	else
	{
		return [the_item childAtIndex:the_index];
	}
}

- (BOOL) outlineView:(NSOutlineView*)outline_view isItemExpandable:(id)the_item
{
	IupCocoaTreeItem* tree_item = (IupCocoaTreeItem*)the_item;

	NSCAssert([tree_item isKindOfClass:[IupCocoaTreeItem class]], @"Expected IupCocoaTreeItem");
		
#if 0
		// This is the basic implementation.
		// The problem is if you add children after this gets called, this happens too late.
		// The workaround would be to use dispatch_async, but that causes a delay and flicker.
		// One other possible solution to to force a reload on the parent item on delete and add node events.
		if([tree_item numberOfChildren] > 0)
		{
			return YES;
		}
		else
		{
			return NO;
		}
#else
		// We are preferring this implementation over the numberOfChildren > 0
		// because when we first add a branch without children, expandItem won't work.
		// The workaround for that is to use dispatch_async, but this causes a delay and flicker.
		// Since IUP makes users declare the difference between a leaf & branch, we can assume all branches should be expandable.
		// And we have that information immediately.
		// The downside "bug" with this implementation is empty branches always show the triangle which is not typical.
		// One other possible solution is to go back to the above or do a hybrid, and try the reload parent idea.
		if([tree_item kind] == ITREE_BRANCH)
		{
#if 0
			// UPDATE1: This might work now an only show the triangle when has children, due to all the other changes I made with adding/deleting.
			// UPDATE2: UPDATE1 seemed to work, but it caused a different problem. For moving (reordering) nodes via drag-and-drop,
			// if a branch is empty, we are unable to move nodes under it.
			// So I think I must keep this path disabled unless there is a different workaround.
			if([tree_item numberOfChildren] > 0)
			{
				return YES;
			}
			else
			{
				return NO;
			}
#else
			return YES;
#endif
		}
		else
		{
			return NO;
		}
#endif
}

/* // Not needed for View based NSOutlineView
- (nullable id)outlineView:(NSOutlineView *)outline_view objectValueForTableColumn:(nullable NSTableColumn *)table_column byItem:(nullable id)the_item
{
	//return (the_item == nil) ? @"/" : @"lower";
	if(nil == the_item)
	{
		return @"Hello World";
	}
	else
	{
		IupCocoaTreeItem* tree_item = (IupCocoaTreeItem*)the_item;
		NSCAssert([tree_item isKindOfClass:[IupCocoaTreeItem class]], @"Expected IupCocoaTreeItem");
		return [tree_item title];
	}
	
}
*/


static NSImage* helperGetActiveImageForTreeItem(IupCocoaTreeItem* tree_item, IupCocoaOutlineView* outline_view, CGFloat* out_width, CGFloat* out_height)
{
	NSImage* active_image = nil;
	int kind = [tree_item kind];
	if(ITREE_BRANCH == kind)
	{
	
		if([outline_view isItemExpanded:tree_item])
		{
			active_image = [tree_item bitmapImage];
			if(nil == active_image)
			{
				active_image = [(IupCocoaOutlineView*)outline_view expandedImage];
			}
		}
		else
		{
			active_image = [tree_item collapsedImage];
			if(nil == active_image)
			{
				active_image = [(IupCocoaOutlineView*)outline_view collapsedImage];
			}
		}
	}
	else if(ITREE_LEAF == kind)
	{
		active_image = [tree_item bitmapImage];
		if(nil == active_image)
		{
			active_image = [(IupCocoaOutlineView*)outline_view leafImage];
		}
	}

	NSSize image_size = NSMakeSize(0.0, 0.0);
	if(active_image)
	{
		image_size = [active_image size];
	}

	if(NULL != out_width)
	{
		*out_width = image_size.width;
	}

	if(NULL != out_height)
	{
		*out_height = image_size.height;
	}
	return active_image;
}

// WARNING: This method needs to be fast for performance.
// I'm worried the image support is already a bit too complicated because it allows for any image size. But I think it will be fine for desktop.
- (CGFloat) outlineView:(NSOutlineView*)outline_view heightOfRowByItem:(id)the_item
{
	IupCocoaTreeItem* tree_item = (IupCocoaTreeItem*)the_item;
	CGFloat text_height = 17.0;
	
	// TODO: Height needs to also account for font if the user changed it
	CGFloat image_width = 0.0;
	CGFloat image_height = 0.0;
	NSImage* active_image = helperGetActiveImageForTreeItem(tree_item, (IupCocoaOutlineView*)outline_view, &image_width, &image_height);
	
	
	// This is a ugly hack keeping the table cell view in the item.
	// All my other attempts to get the table cell view have failed.
	// DOC: If you call viewAtColumn:row:makeIfNecessary: or rowViewAtRow:makeIfNecessary: within your implementation of this method, an exception is thrown.
	// Perhaps I could compute the raw string height if I know the font and hard code a value for the textfield padding.
	NSTableCellView* table_cell_view = [tree_item tableCellView];

	NSTextField* text_field = [table_cell_view textField];
	NSSize text_field_size = { 0, 0 };
	if(text_field)
	{
		text_field_size = [text_field fittingSize];
	}
	
	if(text_field_size.height > text_height)
	{
		text_height = text_field_size.height;
	}
	else if(text_field_size.height == 0)
	{
		// don't allow 0
	}
	else
	{
		// should I allow smaller text heights?
		text_height = text_field_size.height;
	}


	if(active_image)
	{
		if(image_height < text_height)
		{
			return text_height;
		}
		else
		{
			return image_height;
		}
	}
	else
	{
		return text_height;
	}

}

// NSOutlineViewDelegate
// WARNING: This is another method that should be fast for performance.
- (nullable NSView *)outlineView:(NSOutlineView*)outline_view viewForTableColumn:(nullable NSTableColumn*)table_column item:(id)the_item
{
	IupCocoaTreeItem* tree_item = (IupCocoaTreeItem*)the_item;
	NSCAssert([tree_item isKindOfClass:[IupCocoaTreeItem class]], @"Expected IupCocoaTreeItem");
	NSString* string_item = [tree_item title];


	// Get an existing cell with the MyView identifier if it exists
	NSTableCellView* table_cell_view = [outline_view makeViewWithIdentifier:@"IupCocoaTreeTableViewCell" owner:self];

	// There is no existing cell to reuse so create a new one
	if(nil == table_cell_view)
	{
		table_cell_view = [[NSTableCellView alloc] initWithFrame:NSZeroRect];

		// The identifier of the NSTextField instance is set to MyView.
		// This allows the cell to be reused.
		
		[table_cell_view setIdentifier:@"IupCocoaTreeTableViewCell"];
	}

	// table_cell_view is now guaranteed to be valid, either as a reused cell
	// or as a new cell, so set the stringValue of the cell to the
	// nameArray value at row
	NSTextField* text_field = nil;
	text_field = [table_cell_view textField];
	NSImageView* image_view = nil;
	image_view = [table_cell_view imageView];
	[text_field setStringValue:string_item];


	CGFloat image_width = 0.0;
	CGFloat image_height = 0.0;
	NSImage* active_image = helperGetActiveImageForTreeItem(tree_item, (IupCocoaOutlineView*)outline_view, &image_width, &image_height);
	
	if(nil == active_image)
	{
		[image_view setHidden:YES];
		[image_view setImage:nil];
	}
	else
	{
		[image_view setHidden:NO];
		[image_view setImage:active_image];
	}

	[tree_item setTableCellView:table_cell_view]; // kind of a hack. We need it to compute the size in heightOf
 
	// Return the result
	return table_cell_view;
}

// I think it is a really bad idea to change images based on expanded or closed
// because this creates another potential reload (which loses selection data)
// and another potential performance bottleneck.
// This is not a typical Mac/Cocoa behavior.
// But the IUP API demands it.
// There is an optimization here to not swap images if the image is the same.
// However, this is a pointer comparison and requires both the user and IUP implementation to not accidentally load the same image twice
// or create two separate object wrappers around the same image.
// I recommend we add something to the official API documation that separate images is a bad idea.
- (void) outlineViewItemWillExpand:(NSNotification*)the_notification
{
	IupCocoaOutlineView* outline_view = (IupCocoaOutlineView*)[the_notification object];
		NSDictionary* user_info = [the_notification userInfo];
	IupCocoaTreeItem* tree_item = (IupCocoaTreeItem*)[user_info objectForKey:@"NSObject"];
//	Ihandle* ih = [outline_view ih];
	
	if(nil == tree_item)
	{
		return;
	}
	


	NSImage* expanded_image = [tree_item bitmapImage];
	NSImage* collapsed_image = [tree_item collapsedImage];
	NSImage* fallback_expanded_image = [outline_view expandedImage];
	NSImage* fallback_collapsed_image = [outline_view collapsedImage];

	if(expanded_image || fallback_expanded_image)
	{
		NSImage* which_expanded_image = nil;
		if(expanded_image)
		{
			which_expanded_image = expanded_image;
		}
		else
		{
			which_expanded_image = fallback_expanded_image;
		}
		NSImage* which_collapsed_image = nil;
		if(collapsed_image)
		{
			which_collapsed_image = collapsed_image;
		}
		else
		{
			which_collapsed_image = fallback_collapsed_image;
		}

		// Only reload if the expanded and collapsed images are different
		// (I'm worried that switching images is not a typical Mac behavior and may hurt built-in performance optimizations.
		// Also, reloading may reset selection which is not nice.)
		// Note: I've been using retain, hoping that we just have a simple pointer comparison and it will avoid doing slow pixel comparisons.
		if(![which_expanded_image isEqual:which_collapsed_image])
		{
			cocoaTreeReloadItem(tree_item, outline_view);
		}
	}
	

}

// I think it is a really bad idea to change images based on expanded or closed
// because this creates another potential reload (which loses selection data)
// and another potential performance bottleneck.
// This is not a typical Mac/Cocoa behavior.
// But the IUP API demands it.
// There is an optimization here to not swap images if the image is the same.
// However, this is a pointer comparison and requires both the user and IUP implementation to not accidentally load the same image twice
// or create two separate object wrappers around the same image.
// I recommend we add something to the official API documation that separate images is a bad idea.
- (void) outlineViewItemWillCollapse:(NSNotification*)the_notification
{

	IupCocoaOutlineView* outline_view = [the_notification object];
	NSDictionary* user_info = [the_notification userInfo];
	IupCocoaTreeItem* tree_item = (IupCocoaTreeItem*)[user_info objectForKey:@"NSObject"];
//	Ihandle* ih = [(IupCocoaOutlineView*)outline_view ih];
	
	if(nil == tree_item)
	{
		return;
	}
	
	
	
	NSImage* expanded_image = [tree_item bitmapImage];
	NSImage* collapsed_image = [tree_item collapsedImage];
	NSImage* fallback_expanded_image = [outline_view expandedImage];
	NSImage* fallback_collapsed_image = [outline_view collapsedImage];

	if(collapsed_image || fallback_collapsed_image)
	{
		NSImage* which_expanded_image = nil;
		if(expanded_image)
		{
			which_expanded_image = expanded_image;
		}
		else
		{
			which_expanded_image = fallback_expanded_image;
		}
		NSImage* which_collapsed_image = nil;
		if(collapsed_image)
		{
			which_collapsed_image = collapsed_image;
		}
		else
		{
			which_collapsed_image = fallback_collapsed_image;
		}

		// Only reload if the expanded and collapsed images are different
		// (I'm worried that switching images is not a typical Mac behavior and may hurt built-in performance optimizations.
		// Also, reloading may reset selection which is not nice.)
		// Note: I've been using retain, hoping that we just have a simple pointer comparison and it will avoid doing slow pixel comparisons.
		if(![which_expanded_image isEqual:which_collapsed_image])
		{
			cocoaTreeReloadItem(tree_item, outline_view);
		}
	}
	


}

- (void) handleSelectionDidChange:(NSOutlineView*)outline_view
{
	// Rules:
	// If we are in single selection mode, we use the single_cb
	// If we are in multiple selection mode, then one of the following:
	// - If it is a single selection, then use single_cb
	// - If it is a multiple selection, then use multi_cb and skip single_cb
	//     - but if multi_cb is not defined, invoke multiple callbacks of single_cb
	// Also, we need to remember that to do multi-unselection_cb.
	// - This fires if there was a multi-selection from the last time.
	
	// Additional notes:
	
	// (1) Multi_cb is supposed to be a contiguous range for a single action.
	// While I expect that will be the typical case on Mac,
	// I do worry that Apple may have or introduce some built-in key-shortcut (e.g. cmd-a selects all) that can create a non-contiguous selection.
	// For example, what if there is an "invert selection" option?
	// So say you pick the middle item. Then you invert it which gets you everything except the middle item.
	// You now have a discontinuous selection created in one-shot.

	// (2) Delete nodes and selection.
	// I asked Scuri if we are supposed to trigger a selection callback when nodes are deleted,
	// since this will alter the list of selected items.
	// He says that IUP does not do a callback for this case.
	// But this is why I broke this into a helper method, in case it needs to be called directly instead of just on Apple's selection notification.
	// (Apple does not seem to give selection notification callbacks for changes caused by delete or reloadData either.)

	NSCAssert([outline_view isKindOfClass:[IupCocoaOutlineView class]], @"Expected IupCocoaOutlineView");
	Ihandle* ih = [(IupCocoaOutlineView*)outline_view ih];
	if(NULL == ih)
	{
		return;
	}
	

	// May not be the best way to determine callback type since the user can change this on the fly.
//	BOOL in_mulitple_selection_mode = [outline_view allowsMultipleSelection];
	IFnii single_selection_cb = (IFnii)IupGetCallback(ih, "SELECTION_CB");
    IFnIi multi_selection_cb = (IFnIi)IupGetCallback(ih, "MULTISELECTION_CB");
    IFnIi multi_unselection_cb = (IFnIi)IupGetCallback(ih, "MULTIUNSELECTION_CB");

	// No sense doing any work if callbacks are not set.
	// NOTE: setting previousSelection will also be skipped in this case.
	if((single_selection_cb == NULL) && (multi_selection_cb == NULL) && (multi_unselection_cb == NULL))
	{
		return;
	}

#if 0
	// debug: print all currently selected items
	{
		NSIndexSet* selected_index = [outline_view selectedRowIndexes];
		NSUInteger selected_i = [selected_index firstIndex];
		while(selected_i != NSNotFound)
		{
			id selected_item = [outline_view itemAtRow:selected_i];
			NSLog(@"all selected_item: %@", [selected_item title]);
			// get the next index in the set
			selected_i = [selected_index indexGreaterThanIndex:selected_i];
		}
	}
#endif
	
	// First handle the unselections
	{
		// We will get a copy of the previous selections.
		NSMutableIndexSet* unselected_set = [previousSelections mutableCopy];
		if(unselected_set != nil)
		{
			// Then remove the current selections from the previous selections.
			// This will leave the unselected items.
			[unselected_set removeIndexes:[outline_view selectedRowIndexes]];
			
			NSUInteger number_of_items = [unselected_set count];

			// If the previous selection had more than 1, it was a multi-selection.
			// That means we need to do a multi-unselection to balance it.
			// This is safer than testing for whether the outlineview is in multi-mode or not because the user could have changed it on the fly.
			if(number_of_items == 0)
			{
				// do nothing
			}
			else if([previousSelections count] > 1)
			{
				if((NULL != multi_unselection_cb) || (NULL != single_selection_cb))
				{
					// We are in the multiple unselection case
					// VLA
					int array_of_ids[number_of_items];
					NSUInteger selected_i = [unselected_set firstIndex];
					size_t i = 0;
					while(selected_i != NSNotFound)
					{
						NSCAssert(i<number_of_items, @"Overflow: More indexes than expected.");

						array_of_ids[i] = (int)selected_i;
						i++;
						// id selected_item = [outline_view itemAtRow:selected_i];
						//	NSLog(@"removed selected_item: %@", [selected_item title]);
						// get the next index in the set
						selected_i = [unselected_set indexGreaterThanIndex:selected_i];
					}

					if(NULL != multi_unselection_cb)
					{
						multi_unselection_cb(ih, array_of_ids, (int)number_of_items);
					}
					else if(NULL != single_selection_cb)
					{
						for(size_t j=0; j<number_of_items; j++)
						{
							single_selection_cb(ih, array_of_ids[j], 1);
						}
					}
				}

			
			}
			else
			{
				// We are in the single unselection case
				// This should be 0, but just in case there are more, use the first one.
				if(number_of_items > 0)
				{
					if(NULL != single_selection_cb)
					{
						NSUInteger selected_i = [unselected_set firstIndex];
			           	single_selection_cb(ih, (int)selected_i, 0);
					}
				}
				// else do nothing since there was nothing unselected

			}
			
			[unselected_set release];
		}
	} // end unselections
	
	
	
	// handle the selections
	{
		// Get a copy of all the current selections
		NSMutableIndexSet* added_selected_set = [[outline_view selectedRowIndexes] mutableCopy];
		// Subtract out the previousSelections from the current selections which leaves just the newly added selections.
		[added_selected_set removeIndexes:previousSelections];

		if(added_selected_set != nil)
		{
			NSUInteger number_of_items = [added_selected_set count];

			if(number_of_items == 0)
			{
				// do nothing
			}
			else if(number_of_items > 1)
			{
				if((NULL != multi_selection_cb) || (NULL != single_selection_cb))
				{
					// We are in the multiple selection case
					// VLA
					int array_of_ids[number_of_items];
					NSUInteger selected_i = [added_selected_set firstIndex];
					size_t i = 0;
					while(selected_i != NSNotFound)
					{
						NSCAssert(i<number_of_items, @"Overflow: More indexes than expected.");

						array_of_ids[i] = (int)selected_i;
						i++;
						// id selected_item = [outline_view itemAtRow:selected_i];
						// NSLog(@"added selected_item: %@", [selected_item title]);
						// get the next index in the set
						selected_i = [added_selected_set indexGreaterThanIndex:selected_i];
					}

					if(NULL != multi_selection_cb)
					{
						multi_selection_cb(ih, array_of_ids, (int)number_of_items);
					}
					else if(NULL != single_selection_cb)
					{
						for(size_t j=0; j<number_of_items; j++)
						{
							single_selection_cb(ih, array_of_ids[j], 1);
						}
					}
				}
			}
			else // number_of_items == 1
			{
				// We are in the single selection case

				if(NULL != single_selection_cb)
				{
					NSUInteger selected_i = [added_selected_set firstIndex];
					single_selection_cb(ih, (int)selected_i, 1);
				}
			}
			
			[added_selected_set release];
		}
	} // end selections
	
	
	// Release the old previousSelections and save the new/current selections as previousSelections for the next time this is called.
	[previousSelections release];
	previousSelections = [[outline_view selectedRowIndexes] copy];
}

- (void) outlineViewSelectionDidChange:(NSNotification*)the_notification
{
	NSOutlineView* outline_view = [the_notification object];
	[self handleSelectionDidChange:outline_view];

}

- (NSIndexSet*) outlineView:(NSOutlineView*)outline_view selectionIndexesForProposedSelection:(NSIndexSet*)proposed_selection_indexes
{
	Ihandle* ih = [(IupCocoaOutlineView*)outline_view ih];
	if(iupAttribGetBoolean(ih, "CANFOCUS"))
	{
		return proposed_selection_indexes;
	}
	else
	{
		return nil;
	}
}


// Drag and drop
// Cocoa makes you do overkill for moving/copying nodes via drag-and-drop and you must implement full blown pasteboard support.
// General idea here:
// https://stackoverflow.com/questions/42315288/how-to-create-nspasteboardwriting-for-drag-and-drop-in-nscollectionview
// Also, suggestions for putting a pointer in NSData
// https://stackoverflow.com/questions/38890174/use-nsvalue-to-wrap-a-c-pointer

static NSData* helperDataWithValue(NSValue* the_value)
{
    NSUInteger size;
    const char* encoding = [the_value objCType];
    NSGetSizeAndAlignment(encoding, &size, NULL);

    void* ptr = malloc(size);
    [the_value getValue:ptr];
    NSData* ret_data = [NSData dataWithBytes:ptr length:size];
    free(ptr);

    return ret_data;
}

- (id <NSPasteboardWriting>)outlineView:(NSOutlineView *)outlineView pasteboardWriterForItem:(id)the_item
{
    // No dragging if <some condition isn't met>
	if(![the_item isKindOfClass:[IupCocoaTreeItem class]])
	{
		return nil;
	}
	IupCocoaTreeItem* tree_item = (IupCocoaTreeItem*)the_item;
	
	
//    Book *book = (Book *)(((NSTreeNode *)item).representedObject);
//    NSString *identifier = book.title;
	
    NSPasteboardItem* paste_board_item = [[NSPasteboardItem alloc] init];
    [paste_board_item autorelease];
//    NSString* unique_id_string = [NSString stringWithFormat:@"%p", tree_item];
//    [paste_board_item setString:unique_id_string forType:IUPCOCOA_OUTLINEVIEW_DRAGANDDROP_TYPE];

	NSValue* pointer_value = [NSValue valueWithPointer:tree_item];
	NSData* data_value = helperDataWithValue(pointer_value);
	
	// archivedDataWithRootObject throws an exception using a pointer
//	NSData* data_value = [NSKeyedArchiver archivedDataWithRootObject:pointer_value];
    [paste_board_item setData:data_value forType:IUPCOCOA_OUTLINEVIEW_DRAGANDDROP_TYPE];
    return paste_board_item;
}


- (void)outlineView:(NSOutlineView *)outlineView draggingSession:(NSDraggingSession *)session willBeginAtPoint:(NSPoint)screenPoint forItems:(NSArray *)dragged_items
{
	[self setItemBeingDragged:nil];

    // If only one item is being dragged, mark it so we can reorder it with a special pboard indicator
    if([dragged_items count] == 1)
    {
    	[self setItemBeingDragged:[dragged_items lastObject]];
	}
}


- (NSDragOperation)outlineView:(NSOutlineView *)outline_view validateDrop:(id < NSDraggingInfo >)drag_info proposedItem:(id)target_item proposedChildIndex:(NSInteger)child_index
{
	NSArray<NSPasteboardType>* drag_types = [[drag_info draggingPasteboard] types];
	
    if([drag_types containsObject:IUPCOCOA_OUTLINEVIEW_DRAGANDDROP_TYPE])
    {
		// If the sender is ourselves, then we accept it as a move or copy, depending on the modifier key
        if([drag_info draggingSource] == outline_view)
        {
            if([drag_info draggingSourceOperationMask] == NSDragOperationCopy)
            {
				BOOL can_drag = child_index >= 0 && target_item;
				if(can_drag)
				{
                	return NSDragOperationCopy;
				}
				else
				{
					// This seems to prevent copying under a leaf which would force us to convert branches to leaves, which I don't think IUP allows.
					return NSDragOperationNone;
				}
            }
            else
            {
            	BOOL can_drag = child_index >= 0 && target_item;

				if(can_drag)
				{
					//	NSInteger index_of_target_item = [outline_view rowForItem:target_item];

					// This code (from Apple example) seems to be to prevent dragging a branch as a child of itself (which will throw an exception if allowed).
					if([self itemBeingDragged])
					{
						// We have a single item being dragged to move; validate if we can move it or not
						// A move is only valid if the target isn't a child of the thing being dragged. We validate that now
						id item_walker = target_item;
						while(item_walker)
						{
							if(item_walker == [self itemBeingDragged])
							{
								return NSDragOperationNone; // Can't do it!
							}
							item_walker = [outline_view parentForItem:item_walker];
						}
						return NSDragOperationMove;
					}
					else
					{
						// For multiple items, what do we do?
						return NSDragOperationNone;
					}
				}
				else
				{
					return NSDragOperationNone;
				}
			}
		}
	}
	return NSDragOperationNone;
}

static void helperMoveNode(IupCocoaOutlineView* outline_view, IupCocoaTreeItem* tree_item, IupCocoaTreeItem* parent_target_tree_item, NSInteger target_child_index)
{
	if(!tree_item)
	{
		return;
	}
	IupCocoaTreeDelegate* data_source_delegate = (IupCocoaTreeDelegate*)[outline_view dataSource];
	IupCocoaTreeItem* parent_tree_item  = [tree_item parentItem];
	
	NSUInteger object_index;
	if(parent_tree_item)
	{
		object_index = [[parent_tree_item childrenArray] indexOfObject:tree_item];

	}
	else
	{
		object_index = [[data_source_delegate treeRootTopLevelObjects] indexOfObject:tree_item];
	}
	
	// If the destination node is a branch and it is expanded,
	// then the specified node is inserted as the first child of the destination node.
	// If the branch is not expanded or the destination node is a leaf,
	// then it is inserted as the next brother of the leaf.
	
//	BOOL is_target_expanded = NO;
//	int target_kind = [target_tree_item kind];
	
	NSInteger adjusted_index = target_child_index;

	// If the node is being moved under the same immediate parent,
	// we need to subtract 1 if
	// the current placement of the node is earlier than the target.
	// because the node's current placement counts against us
	// and target_child_index is +1 too much.
	if([parent_tree_item isEqual:parent_target_tree_item])
	{
		if(object_index < target_child_index)
		{
			adjusted_index = target_child_index - 1;
		}
	}
	if(adjusted_index < 0)
	{
		adjusted_index = 0;
	}
	
	// update the data source
	[data_source_delegate moveItem:tree_item targetParent:parent_target_tree_item targetChildIndex:adjusted_index];
	[outline_view moveItemAtIndex:object_index inParent:parent_tree_item toIndex:adjusted_index inParent:parent_target_tree_item];
}

// tree_item should be a copy
static void helperCopyAndInsertNode(IupCocoaOutlineView* outline_view, IupCocoaTreeItem* tree_item, IupCocoaTreeItem* parent_target_tree_item, NSInteger target_child_index, NSTableViewAnimationOptions copy_insert_animation)
{
	if(!tree_item)
	{
		return;
	}
	IupCocoaTreeDelegate* data_source_delegate = (IupCocoaTreeDelegate*)[outline_view dataSource];

	//IupCocoaTreeItem* new_copy_tree_item = [[IupCocoaTreeItem alloc] init];
	IupCocoaTreeItem* new_copy_tree_item = [tree_item cloneWithNewParentItem:parent_target_tree_item];
		// If the destination node is a branch and it is expanded,
	// then the specified node is inserted as the first child of the destination node.
	// If the branch is not expanded or the destination node is a leaf,
	// then it is inserted as the next brother of the leaf.
	
//	BOOL is_target_expanded = NO;
//	int target_kind = [target_tree_item kind];
	
	// Unlike move, we don't seem to need to subtract 1.
//	NSInteger adjusted_index = target_child_index - 1;
	NSInteger adjusted_index = target_child_index;
	if(adjusted_index < 0)
	{
		adjusted_index = 0;
	}
	
	// update the data source
	[data_source_delegate insertChild:new_copy_tree_item withParent:parent_target_tree_item targetChildIndex:adjusted_index];

	// directly update the outlineview so we don't have to reloadData
	NSIndexSet* index_set = [NSIndexSet indexSetWithIndex:adjusted_index];
	[outline_view insertItemsAtIndexes:index_set inParent:parent_target_tree_item withAnimation:copy_insert_animation];
	
}

static IupCocoaTreeItem* helperRecursiveIsPointerValid(intptr_t look_for_pointer, IupCocoaTreeItem* current_item)
{
	if(look_for_pointer == (intptr_t)current_item)
	{
		return current_item;
	}
	
	for(IupCocoaTreeItem* a_item in [current_item childrenArray])
	{
		IupCocoaTreeItem* is_found = helperRecursiveIsPointerValid(look_for_pointer, a_item);
		if(is_found)
		{
			return is_found;
		}
	}
	return NULL;
}

static IupCocoaTreeItem* helperIsPointerValid(intptr_t look_for_pointer, IupCocoaTreeDelegate* tree_delegate)
{
	for(IupCocoaTreeItem* tree_item in [tree_delegate treeRootTopLevelObjects])
	{
		IupCocoaTreeItem* is_found = helperRecursiveIsPointerValid(look_for_pointer, tree_item);
		if(is_found)
		{
			return is_found;
		}
	}
	return NULL;
}

- (BOOL) outlineView:(NSOutlineView*)outline_view acceptDrop:(id <NSDraggingInfo>)drag_info item:(id)parent_target_tree_item childIndex:(NSInteger)target_child_index
{
	[self setItemBeingDragged:nil];
    NSPasteboard* paste_board = [drag_info draggingPasteboard];
	NSData* data_value = [paste_board dataForType:IUPCOCOA_OUTLINEVIEW_DRAGANDDROP_TYPE];
	if(nil == data_value)
	{
		return NO;
	}
	
//	NSValue* pointer_value = [NSKeyedUnarchiver unarchiveObjectWithData:data_value];
//	IupCocoaTreeItem* tree_item = (IupCocoaTreeItem*)[pointer_value pointerValue];

	intptr_t decoded_integer;
	[data_value getBytes:&decoded_integer length:sizeof(intptr_t)];

	// Could be a wild pointer at this point?
	// We can iterate through all the nodes to find it if we need to be extra safe.
//	IupCocoaTreeItem* tree_item = (IupCocoaTreeItem*)decoded_integer;
	IupCocoaTreeDelegate* data_source_delegate = (IupCocoaTreeDelegate*)[outline_view dataSource];
	IupCocoaTreeItem* tree_item = helperIsPointerValid(decoded_integer, data_source_delegate);
	if(nil == tree_item)
	{
		return NO;
	}


	// NOTE: Apple has this in their example. I still don't understand what this does.
    // If it was a drop "on", then we add it at the start
    if(target_child_index == NSOutlineViewDropOnItemIndex)
    {
        target_child_index = 0;
    }
    [outline_view beginUpdates];


    // Are we copying the data or moving something?
    if([drag_info draggingSourceOperationMask] == NSDragOperationCopy)
    {
        // Yes, this is an insert from the pasteboard (even if it is a copy of itemBeingDragged)
		helperCopyAndInsertNode((IupCocoaOutlineView*)outline_view, tree_item, parent_target_tree_item, target_child_index, NSTableViewAnimationEffectGap);
    }
    else
    {
    	helperMoveNode((IupCocoaOutlineView*)outline_view, tree_item, parent_target_tree_item, target_child_index);
    }

	[outline_view endUpdates];

    return YES;
}


@end

/*****************************************************************************/
/* ADDING ITEMS                                                              */
/*****************************************************************************/

static void cocoaTreeReloadItem(IupCocoaTreeItem* tree_item, NSOutlineView* outline_view)
{
	NSOperatingSystemVersion macosx_1012 = { 10, 12, 0 };
	
	// isOperatingSystemAtLeastVersion officially requires 10.10+, but seems available on 10.9
	if([[NSProcessInfo processInfo] isOperatingSystemAtLeastVersion:macosx_1012])
	{
		// Starting in 10.12, reloadItem: was fixed to do the right thing. Must link to 10.12 SDK or higher (which you should always link to the lastest on Mac anyway)
		[outline_view reloadItem:tree_item];
	}
	else
	{
		[outline_view reloadData];
	}
}

void iupdrvTreeAddNode(Ihandle* ih, int prev_id, int kind, const char* title, int add)
{
/*
 
 id is the number identifier of a reference node, the reference node is used to position the new node.
 
 kind is the new node type, if it is a branch or a leaf.
 
 add means appending a node at the end of the branch, if 0 means inserting the node in the branch
 
 If the reference node exists then
 if (reference node is a branch and appending)
 insert the new node after the reference node, as first child
 else
 insert the new node after reference node
 else
 add the new node at root
*/
	NSOutlineView* outline_view = cocoaTreeGetOutlineView(ih);
	IupCocoaTreeDelegate* data_source_delegate = (IupCocoaTreeDelegate*)[outline_view dataSource];
	
	

	
	
	InodeHandle* inode_prev = iupTreeGetNode(ih, prev_id);
	

	/* the previous node is not necessary only
	 if adding the root in an empty tree or before the root. */
	// UPDATE: This check is breaking things for me.
/*
	if (!inode_prev && prev_id!=-1)
	{
		return;
	}
*/

	if (!title)
	{
		title = "";
	}
	

	
	IupCocoaTreeItem* tree_item_new = [[IupCocoaTreeItem alloc] init];
	[tree_item_new setKind:kind];
	NSString* ns_title = [NSString stringWithUTF8String:title];
	[tree_item_new setTitle:ns_title];
//	InodeHandle* inode_new = (InodeHandle*)calloc(1, sizeof(InodeHandle));
	InodeHandle* inode_new = (InodeHandle*)tree_item_new; // NOTE: retain count is 1 from alloc. We are not going to retain it again.
	
	//  If the reference node exists then
	if(inode_prev)
	{
		IupCocoaTreeItem* tree_item_prev = inode_prev;
		int kind_prev = [tree_item_prev kind];
	
		
		// if (reference node is a branch and appending)
		if((ITREE_BRANCH == kind_prev) && (1 == add))
		{
			// insert the new node after the reference node, as first child
			/* depth+1 */
			// IUP always inserts the child in the first position, not the last
			// update the data source
			NSUInteger target_index = [data_source_delegate insertChild:tree_item_new withParent:tree_item_prev];

			// directly update the outlineview so we don't have to reloadData
			NSIndexSet* index_set = [NSIndexSet indexSetWithIndex:target_index];
			[outline_view insertItemsAtIndexes:index_set inParent:tree_item_prev withAnimation:NSTableViewAnimationEffectNone];

		}
		else
		{
			// insert the new node after reference node
			/* same depth */
			
			// update the data source
			NSUInteger target_index = [data_source_delegate insertPeer:tree_item_new withSibling:tree_item_prev];

			// directly update the outlineview so we don't have to reloadData
			// It is okay if the parent is nil. This also handles the case where ADDROOT=NO and we are adding another top-level node.
			IupCocoaTreeItem* tree_item_parent = [tree_item_prev parentItem];
			NSIndexSet* index_set = [NSIndexSet indexSetWithIndex:target_index];
			[outline_view insertItemsAtIndexes:index_set inParent:tree_item_parent withAnimation:NSTableViewAnimationEffectNone];

		}

		iupTreeAddToCache(ih, add, kind_prev, inode_prev, inode_new);

	}
	else
	{
		//  add the new node at root
		[data_source_delegate insertAtRoot:tree_item_new];
		// directly update the outlineview so we don't have to reloadData
		[outline_view insertItemsAtIndexes:[NSIndexSet indexSetWithIndex:0] inParent:nil withAnimation:NSTableViewAnimationEffectNone];

		iupTreeAddToCache(ih, 0, 0, NULL, inode_new);

	}
	// We don't need to reloadData if we update the outline view directly.
//	[outline_view reloadData];

	if(ITREE_BRANCH == kind)
	{
		BOOL should_expand = IupGetInt(ih, "ADDEXPANDED");
		if(should_expand)
		{
			// Just in case we do have children already, expand now which may skip the animation delay.
			//[outline_view expandItem:tree_item_new];
			[outline_view expandItem:tree_item_new expandChildren:YES];
#if 0
			// Tricky: This wasn't working until I added dispatch_async.
			// I think the problem is that when I expand a branch, the children may not be added yet.
			// So if there are no children at the time, my expand request gets ignored.
			// The dispatch_async will force the expand to happen on the next event loop pass, after any children have been added from this loop.
			// So the expand will work now that the children exist.
			// UPDATE: This is now fixed by using
			// - (BOOL) outlineView:(NSOutlineView*)outline_view isItemExpandable:(id)the_item
			// and making it rely on "kind" to determine if expandable instead of number of children.
			dispatch_async(dispatch_get_main_queue(), ^{
//				[outline_view expandItem:tree_item_new];
				[outline_view expandItem:tree_item_new expandChildren:YES];
				}
			);
#endif
		}
	}
	
	// make sure to release since it should now be retained by the data_source_delegate
	[tree_item_new release];
}



int iupdrvTreeTotalChildCount(Ihandle* ih, InodeHandle* node_handle)
{

	IupCocoaTreeItem* tree_item = (IupCocoaTreeItem*)node_handle;
//	NSUInteger number_of_items = [tree_item numberOfChildren];

	NSUInteger number_of_items = Helper_RecursivelyCountItems(tree_item);
	// subtract one because we don't include 'this' node as part of the children count
	if(number_of_items > 0)
	{
		number_of_items = number_of_items - 1;
	}
	return (int)number_of_items;
}

InodeHandle* iupdrvTreeGetFocusNode(Ihandle* ih)
{
	NSOutlineView* outline_view = cocoaTreeGetOutlineView(ih);

	id selected_item = [outline_view itemAtRow:[outline_view selectedRow]];

	
	
	return (InodeHandle*)selected_item;
}


// FIXME: Why does the GTK version look so different?
void iupdrvTreeUpdateMarkMode(Ihandle *ih)
{
	NSOutlineView* outline_view = cocoaTreeGetOutlineView(ih);
	const char* mark_mode = iupAttribGet(ih, "MARKMODE");
	if(iupStrEqualNoCase(mark_mode, "MULTIPLE"))
	{
		[outline_view setAllowsMultipleSelection:YES];
	}
	else
	{
		[outline_view setAllowsMultipleSelection:NO];
	}
}



void iupdrvTreeDragDropCopyNode(Ihandle* src, Ihandle* dst, InodeHandle *itemSrc, InodeHandle *itemDst)
{
	
}


static int cocoaTreeSetDelNodeAttrib(Ihandle* ih, int node_id, const char* value)
{
	if (!ih->handle)  /* do not do the action before map */
		return 0;

	NSOutlineView* outline_view = cocoaTreeGetOutlineView(ih);
	IupCocoaTreeDelegate* data_source_delegate = (IupCocoaTreeDelegate*)[outline_view dataSource];


	if (iupStrEqualNoCase(value, "ALL"))
	{
		NSUInteger number_of_root_items = [[data_source_delegate treeRootTopLevelObjects] count];
		if(number_of_root_items > 0)
		{

			[data_source_delegate removeAllObjects];

			// Scuri says not required to handle for delete.
			// [data_source_delegate handleSelectionDidChange:outline_view];

			// If there are multiple nodes at the root (ADDROOT=NO), it seems easier to reloadData than to hunt down and remove each node.
			[outline_view reloadData];

		}

		return 0;
	}
	if (iupStrEqualNoCase(value, "SELECTED"))  /* selected here means the reference node */
	{

		IupCocoaTreeItem* tree_item = (IupCocoaTreeItem*)iupTreeGetNode(ih, node_id);

		if(!tree_item)
		{
			return 0;
		}
		NSCAssert([tree_item isKindOfClass:[IupCocoaTreeItem class]], @"expecting class IupCocoaTreeItem");

		[outline_view beginUpdates];

		IupCocoaTreeItem* parent_tree_item = [tree_item parentItem]; // get parent before removing because it may nil out the parent in removeItem
		NSUInteger target_index = [data_source_delegate removeItem:tree_item];
		//	[outline_view reloadData];

		if(NSNotFound != target_index)
		{
			NSIndexSet* index_set = [NSIndexSet indexSetWithIndex:target_index];
			[outline_view removeItemsAtIndexes:index_set inParent:parent_tree_item withAnimation:NSTableViewAnimationEffectNone];
		}

		// Scuri says not required to handle for delete.
		// [data_source_delegate handleSelectionDidChange:outline_view];

		[outline_view endUpdates];

	}
	else if(iupStrEqualNoCase(value, "CHILDREN"))  /* children of the reference node */
	{
		IupCocoaTreeItem* tree_item = (IupCocoaTreeItem*)iupTreeGetNode(ih, node_id);

		if(!tree_item)
		{
			return 0;
		}
		NSCAssert([tree_item isKindOfClass:[IupCocoaTreeItem class]], @"expecting class IupCocoaTreeItem");
		
		[outline_view beginUpdates];

		NSIndexSet* index_set = [data_source_delegate removeAllChildrenForItem:tree_item];
		[outline_view removeItemsAtIndexes:index_set inParent:tree_item withAnimation:NSTableViewAnimationEffectNone];

		//[outline_view reloadData];

		// Scuri says not required to handle for delete.
		// [data_source_delegate handleSelectionDidChange:outline_view];

		[outline_view endUpdates];

		return 0;
		
	}
	else if(iupStrEqualNoCase(value, "MARKED"))  /* Delete the array of marked nodes */
	{
		[outline_view beginUpdates];
		NSIndexSet* selected_index = [outline_view selectedRowIndexes];

		NSUInteger selected_i = [selected_index firstIndex];
		while(selected_i != NSNotFound)
		{
			id selected_item = [outline_view itemAtRow:selected_i];

			// I can't figure out how to make this work correctly when you select both parents and its children to be deleted.
			// Use reloadData for now.
#if 0
			IupCocoaTreeItem* parent_tree_item  = [(IupCocoaTreeItem*)selected_item parentItem];
			NSUInteger target_index = [data_source_delegate removeItem:selected_item];
			
			NSIndexSet* index_set = [NSIndexSet indexSetWithIndex:target_index];
			[outline_view removeItemsAtIndexes:index_set inParent:parent_tree_item withAnimation:NSTableViewAnimationEffectNone];
#else
			[data_source_delegate removeItem:selected_item];
#endif

			
			// get the next index in the set
			selected_i = [selected_index indexGreaterThanIndex:selected_i];
		}

		[outline_view reloadData];

		// Scuri says not required to handle for delete.
		// [data_source_delegate handleSelectionDidChange:outline_view];
		[outline_view endUpdates];

	}

	return 0;
}


static int cocoaTreeSetMoveNodeAttrib(Ihandle* ih, int node_id, const char* value)
{


	if(!ih->handle)  /* do not do the action before map */
	{
		return 0;
	}

	IupCocoaOutlineView* outline_view = (IupCocoaOutlineView*)cocoaTreeGetOutlineView(ih);
	IupCocoaTreeDelegate* data_source_delegate = (IupCocoaTreeDelegate*)[outline_view dataSource];

	IupCocoaTreeItem* tree_item = (IupCocoaTreeItem*)iupTreeGetNode(ih, node_id);

	if(!tree_item)
	{
		return 0;
	}
	IupCocoaTreeItem* parent_tree_item  = [tree_item parentItem];

	IupCocoaTreeItem* target_tree_item = (IupCocoaTreeItem*)iupTreeGetNodeFromString(ih, value);
	IupCocoaTreeItem* parent_target_tree_item = [target_tree_item parentItem];

	/* If Drag item is an ancestor of Drop item then return */
	if([tree_item isEqual:target_tree_item])
	{
		return 0;
	}
#if 0
	if([tree_item isEqual:parent_target_tree_item])
	{
		return 0;
	}
#endif

	NSUInteger object_index;
	if(parent_tree_item)
	{
		object_index = [[parent_tree_item childrenArray] indexOfObject:tree_item];

	}
	else
	{
		object_index = [[data_source_delegate treeRootTopLevelObjects] indexOfObject:tree_item];
	}

	// If the destination node is a branch and it is expanded,
	// then the specified node is inserted as the first child of the destination node.
	// If the branch is not expanded or the destination node is a leaf,
	// then it is inserted as the next brother of the leaf.
	
	BOOL is_target_expanded = NO;
	int target_kind = [target_tree_item kind];
	if(ITREE_BRANCH == target_kind)
	{
		is_target_expanded = [outline_view isItemExpanded:target_tree_item];


		if(is_target_expanded)
		{
		

			// update the data source
			
			helperMoveNode(outline_view, tree_item, parent_target_tree_item, 0);


		}
		else
		{
			NSUInteger target_index;
			if(parent_target_tree_item)
			{
				target_index = [[parent_target_tree_item childrenArray] indexOfObject:target_tree_item];
			}
			else
			{
				target_index = [[data_source_delegate treeRootTopLevelObjects] indexOfObject:target_tree_item];
			}
			// update the data source
			helperMoveNode(outline_view, tree_item, parent_target_tree_item, target_index);

		}
	}
	else
	{
		NSUInteger target_index;
		if(parent_target_tree_item)
		{
			target_index = [[parent_target_tree_item childrenArray] indexOfObject:target_tree_item];
		}
		else
		{
			target_index = [[data_source_delegate treeRootTopLevelObjects] indexOfObject:target_tree_item];
		}
		// update the data source
		helperMoveNode(outline_view, tree_item, parent_target_tree_item, target_index);

	
	}
	
	


  return 0;
}

/*****************************************************************************/
/* MANIPULATING IMAGES                                                       */
/*****************************************************************************/

/* Apple has a built-in Folder image we are supposed to use.
	Also, the default IUP one looks awful on Mac. (The resolution is too low and the colors are wrong.)
 	So we will override the default IUP one here.
 	IUP has different images for expanded and collaspsed, but that is not a normal thing for Mac.
 	Additionally, there is a performance cost and possible issues with losing selections due to reloading items when we switch on Mac.
 	And Apple does not provide an 'open' folder image.
	So we will make the expanded and collapsed images point to the same image here.
	This will allow us to get back a little performance by default.
 	TODO: Redesign the core iup_tree.c code to not load the default images for performance.
*/
static void helperLoadReplacementDefaultImages()
{
	NSImage* ns_folder_image = [NSImage imageNamed:NSImageNameFolder];

	// The default IUP image 16x16.
	// The default table cell height accomodates height=16 perfectly.
	// But the Apple icon is currently 32x32 (subject to get bigger over time).
	// So we will scale the image down to 16x16.
	CGRect resize_rect = { 0, 0, 16, 16 };
	
	CGImageRef cg_image = [ns_folder_image CGImageForProposedRect:&resize_rect context:nil hints:nil];
//	CGImageRef cg_image = [ns_folder_image CGImageForProposedRect:nil context:nil hints:nil];

	NSUInteger bytes_per_row  = resize_rect.size.width * 4; // rgba
  	NSUInteger total_bytes  = bytes_per_row * resize_rect.size.height;

  	void* copy_pixel_buffer = malloc(total_bytes);
	CGColorSpaceRef color_space = CGColorSpaceCreateDeviceRGB();
	
	 CGContextRef offscreen_context_ref = CGBitmapContextCreate(
		copy_pixel_buffer,
		resize_rect.size.width,
		resize_rect.size.height,
		8, // bits per component
		bytes_per_row,
		color_space,
		kCGImageAlphaPremultipliedLast
	);
	CGColorSpaceRelease(color_space);
	CGContextDrawImage(offscreen_context_ref, resize_rect, cg_image);
	
//	CFDataRef raw_data = CGDataProviderCopyData(CGImageGetDataProvider(offscreen_context_ref));
//	unsigned char* raw_data = CGBitmapContextGetData(offscreen_context_ref);

//	CFIndex data_length = CFDataGetLength(raw_data);
  	Ihandle* ih_folder_image = IupImageRGBA(resize_rect.size.width, resize_rect.size.height, (unsigned char*)copy_pixel_buffer);
//	CFRelease(raw_data);
	CFRelease(offscreen_context_ref);
	free(copy_pixel_buffer);
	// Copied from collapsed_image. I don't know if these colors make sense, or if they are ever used.
#if 1
	IupSetAttribute(ih_folder_image, "0", "BGCOLOR");
	IupSetAttribute(ih_folder_image, "1", "255 206 156");
	IupSetAttribute(ih_folder_image, "2", "156 156 0");
	IupSetAttribute(ih_folder_image, "3", "0 0 0");
	IupSetAttribute(ih_folder_image, "4", "206 206 99");
	IupSetAttribute(ih_folder_image, "5", "255 255 206");
	IupSetAttribute(ih_folder_image, "6", "247 247 247");
	IupSetAttribute(ih_folder_image, "7", "255 255 156");
#endif

	IupSetHandle("IMGCOLLAPSED", ih_folder_image);
	IupSetHandle("IMGEXPANDED",  ih_folder_image);
}

static void helperReplaceDefaultImages(Ihandle* ih, IupCocoaOutlineView* outline_view)
{
	// This was loaded in helperLoadReplacementDefaultImages.
	// Round-tripping the image through IUP is intentional in case IUP does any additional transformations.
	NSImage* ns_folder_image_roundtrip = iupImageGetImage(iupAttribGetStr(ih, "IMAGEBRANCHCOLLAPSED"), ih, 0);
	
	// We use the same pointer for both. This allows for an optimization to kick in to not reload items when expand/collapse happens.
	[outline_view setCollapsedImage:ns_folder_image_roundtrip];
	[outline_view setExpandedImage:ns_folder_image_roundtrip];

}

static NSImage* helperGetImage(Ihandle* ih, int node_id, const char* value, IupCocoaTreeItem* tree_item)
{
	if(!tree_item)
	{
		return nil;
	}

	NSImage* bitmap_image = nil;
	
	if(iupStrEqualNoCase("IMGEMPTY", value))
	{
		bitmap_image = nil;
	}
	else
	{
		bitmap_image = (NSImage*)iupImageGetImage(value, ih, 0);
	}

	return bitmap_image;
}

static int cocoaTreeSetImageExpandedAttrib(Ihandle* ih, int node_id, const char* value)
{
	IupCocoaTreeItem* tree_item = (IupCocoaTreeItem*)iupTreeGetNode(ih, node_id);
	NSOutlineView* outline_view = cocoaTreeGetOutlineView(ih);

	NSImage* bitmap_image = helperGetImage(ih, node_id, value, tree_item);
	
	// Oh how ironic and tragic.
	// I combined expanded and leaf into the same variable and made collapsed separate.
	// But IUP combined collpased and leaf into the same variable and made expanded separate.
	[tree_item setBitmapImage:bitmap_image];
	cocoaTreeReloadItem(tree_item, outline_view);

	return 1;
}

static int cocoaTreeSetImageAttrib(Ihandle* ih, int node_id, const char* value)
{
	IupCocoaTreeItem* tree_item = (IupCocoaTreeItem*)iupTreeGetNode(ih, node_id);
	NSOutlineView* outline_view = cocoaTreeGetOutlineView(ih);

	NSImage* bitmap_image = helperGetImage(ih, node_id, value, tree_item);

	
	// Oh how ironic and tragic.
	// I combined expanded and leaf into the same variable and made collapsed separate.
	// But IUP combined collpased and leaf into the same variable and made expanded separate.
	if([tree_item kind] == ITREE_LEAF)
	{
		[tree_item setBitmapImage:bitmap_image];
	}
	else
	{
		[tree_item setCollapsedImage:bitmap_image];
	}
	cocoaTreeReloadItem(tree_item, outline_view);

	return 0;
}

static void helperSetImageBranchExpanded(IupCocoaOutlineView* outline_view, IupCocoaTreeItem* tree_item, NSImage* ns_image)
{
	for(IupCocoaTreeItem* a_item in [tree_item childrenArray])
	{
		helperSetImageBranchExpanded(outline_view, a_item, ns_image);
	}
	if([tree_item kind] == ITREE_BRANCH)
	{
		// only need to reload if the user hasn't overridden with a custom-per-node image.
		if(![tree_item bitmapImage])
		{
			cocoaTreeReloadItem(tree_item, outline_view);
		}
	}
}

static int cocoaTreeSetImageBranchExpandedAttrib(Ihandle* ih, const char* value)
{
	NSImage* ns_image = (NSImage*)iupImageGetImage(value, ih, 0);
	IupCocoaOutlineView* outline_view = (IupCocoaOutlineView*)cocoaTreeGetOutlineView(ih);

	[outline_view beginUpdates];

	[outline_view setExpandedImage:ns_image];

	/* Update all images */
	IupCocoaTreeDelegate* tree_delegate = (IupCocoaTreeDelegate*)[outline_view dataSource];
	for(IupCocoaTreeItem* tree_item in [tree_delegate treeRootTopLevelObjects])
	{
		helperSetImageBranchExpanded(outline_view, tree_item, ns_image);
	}
	
	[outline_view endUpdates];

	return 1;
}


static void helperSetImageBranchCollapsed(IupCocoaOutlineView* outline_view, IupCocoaTreeItem* tree_item, NSImage* ns_image)
{
	for(IupCocoaTreeItem* a_item in [tree_item childrenArray])
	{
		helperSetImageBranchCollapsed(outline_view, a_item, ns_image);
	}
	// only need to reload if the user hasn't overridden with a custom-per-node image.
	if([tree_item kind] == ITREE_BRANCH)
	{
		// only need to reload if the user hasn't overridden with a custom-per-node image.
		if(![tree_item collapsedImage])
		{
			cocoaTreeReloadItem(tree_item, outline_view);
		}
	}
}

static int cocoaTreeSetImageBranchCollapsedAttrib(Ihandle* ih, const char* value)
{
	NSImage* ns_image = (NSImage*)iupImageGetImage(value, ih, 0);
	IupCocoaOutlineView* outline_view = (IupCocoaOutlineView*)cocoaTreeGetOutlineView(ih);

	[outline_view beginUpdates];

	[outline_view setCollapsedImage:ns_image];

	/* Update all images */
	IupCocoaTreeDelegate* tree_delegate = (IupCocoaTreeDelegate*)[outline_view dataSource];
	for(IupCocoaTreeItem* tree_item in [tree_delegate treeRootTopLevelObjects])
	{
		helperSetImageBranchCollapsed(outline_view, tree_item, ns_image);
	}
	
	[outline_view endUpdates];

	return 1;
}

static void helperSetImageLeaf(IupCocoaOutlineView* outline_view, IupCocoaTreeItem* tree_item, NSImage* ns_image)
{
	for(IupCocoaTreeItem* a_item in [tree_item childrenArray])
	{
		helperSetImageLeaf(outline_view, a_item, ns_image);
	}
	if([tree_item kind] == ITREE_LEAF)
	{
		// only need to reload if the user hasn't overridden with a custom-per-node image.
		if(![tree_item bitmapImage])
		{
			cocoaTreeReloadItem(tree_item, outline_view);
		}
	}
}

static int cocoaTreeSetImageLeafAttrib(Ihandle* ih, const char* value)
{
	NSImage* ns_image = (NSImage*)iupImageGetImage(value, ih, 0);
	IupCocoaOutlineView* outline_view = (IupCocoaOutlineView*)cocoaTreeGetOutlineView(ih);

	[outline_view beginUpdates];

	[outline_view setLeafImage:ns_image];

	/* Update all images */
	IupCocoaTreeDelegate* tree_delegate = (IupCocoaTreeDelegate*)[outline_view dataSource];
	for(IupCocoaTreeItem* tree_item in [tree_delegate treeRootTopLevelObjects])
	{
		helperSetImageLeaf(outline_view, tree_item, ns_image);
	}
	
	[outline_view endUpdates];

	return 1;
}


/*****************************************************************************/
/* AUXILIAR FUNCTIONS                                                        */
/*****************************************************************************/

static char* cocoaTreeGetTitleAttrib(Ihandle* ih, int item_id)
{
//	NSOutlineView* outline_view = cocoaTreeGetOutlineView(ih);
//	IupCocoaTreeDelegate* data_source_delegate = (IupCocoaTreeDelegate*)[outline_view dataSource];
	
	InodeHandle* inode_handle = iupTreeGetNode(ih, item_id);

	if(inode_handle)
	{
		IupCocoaTreeItem* tree_item = (IupCocoaTreeItem*)inode_handle;
		NSCAssert([tree_item isKindOfClass:[IupCocoaTreeItem class]], @"expecting class IupCocoaTreeItem");
		NSString* ns_title = [tree_item title];
		return iupStrReturnStr([ns_title UTF8String]);
	}
	else
	{
		return NULL;
	}
}

static int cocoaTreeSetTitleAttrib(Ihandle* ih, int item_id, const char* value)
{
	InodeHandle* inode_handle = iupTreeGetNode(ih, item_id);
	
	if(inode_handle)
	{
		NSString* ns_title = @"";
		if(value)
		{
			ns_title = [NSString stringWithUTF8String:value];
		}
		
		IupCocoaTreeItem* tree_item = (IupCocoaTreeItem*)inode_handle;
		NSCAssert([tree_item isKindOfClass:[IupCocoaTreeItem class]], @"expecting class IupCocoaTreeItem");
		[tree_item setTitle:ns_title];
		NSOutlineView* outline_view = cocoaTreeGetOutlineView(ih);

		cocoaTreeReloadItem(tree_item, outline_view);
	}

	return 0;
}

static int cocoaTreeSetExpandAllAttrib(Ihandle* ih, const char* value)
{
	NSOutlineView* outline_view = cocoaTreeGetOutlineView(ih);
	bool should_expand = iupStrBoolean(value);
	
	if(should_expand)
	{
		// item=nil should do all root items
		[outline_view expandItem:nil expandChildren:YES];
	}
	else
	{
		// item=nil should do all root items
		[outline_view collapseItem:nil collapseChildren:YES];
	}

  return 0;
}


static int cocoaTreeMapMethod(Ihandle* ih)
{
	NSBundle* framework_bundle = [NSBundle bundleWithIdentifier:@"br.puc-rio.tecgraf.iup"];
	NSNib* outline_nib = [[NSNib alloc] initWithNibNamed:@"IupCocoaOutlineView" bundle:framework_bundle];
	
	
	NSArray* top_level_objects = nil;
	
	
	IupCocoaOutlineView* outline_view = nil;
	NSScrollView* scroll_view = nil;
	
	if([outline_nib instantiateWithOwner:nil topLevelObjects:&top_level_objects])
	{
		for(id current_object in top_level_objects)
		{

			if([current_object isKindOfClass:[NSScrollView class]])
			{
				scroll_view = current_object;
				break;
			}
		}
	}
	
	outline_view = (IupCocoaOutlineView*)[scroll_view documentView];
	NSCAssert([outline_view isKindOfClass:[IupCocoaOutlineView class]], @"Expected IupCocoaOutlineView");
	
	// ScrollView is expected to hold on to all the other objects we need
	[scroll_view retain];
	[outline_nib release];
	
	// We need a way to get the ih during Cocoa callbacks, such as for selection changed notifications.
	[outline_view setIh:ih];

	
	IupCocoaTreeDelegate* tree_delegate = [[IupCocoaTreeDelegate alloc] init];
	
	[outline_view setDataSource:tree_delegate];
	[outline_view setDelegate:tree_delegate];
	

	
	// We're going to use OBJC_ASSOCIATION_RETAIN because I do believe it will do the right thing for us.
	// I'm attaching to the scrollview instead of the outline view because I'm a little worried about circular references and I'm hoping this helps a little
	objc_setAssociatedObject(scroll_view, IUP_COCOA_TREE_DELEGATE_OBJ_KEY, (id)tree_delegate, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
	[tree_delegate release];
	
	ih->handle = scroll_view;
	
	
	
	// All Cocoa views shoud call this to add the new view to the parent view.
	iupCocoaAddToParent(ih);
	
	
	
	// turn off the header
	[outline_view setHeaderView:nil];

	/* Initialize the default images */
	NSImage* leaf_image = iupImageGetImage(iupAttribGetStr(ih, "IMAGELEAF"), ih, 0);
//	NSImage* collapsed_image = iupImageGetImage(iupAttribGetStr(ih, "IMAGEBRANCHCOLLAPSED"), ih, 0);
//	NSImage* expanded_image = iupImageGetImage(iupAttribGetStr(ih, "IMAGEBRANCHEXPANDED"), ih, 0);




	[outline_view setLeafImage:leaf_image];

#if 1
	helperReplaceDefaultImages(ih, outline_view);
#else
	NSImage* collapsed_image = [NSImage imageNamed:NSImageNameFolder];
//	NSImage* expanded_image = [NSImage imageNamed:NSImageNameFolder];
	NSImage* expanded_image = collapsed_image;
	
	[outline_view setCollapsedImage:collapsed_image];
	[outline_view setExpandedImage:expanded_image];
#endif

	if (iupAttribGetInt(ih, "ADDROOT"))
	{
		iupdrvTreeAddNode(ih, -1, ITREE_BRANCH, "", 0);
	}
	
	/* configure for DRAG&DROP of files */
	if (IupGetCallback(ih, "DROPFILES_CB"))
	{
		iupAttribSet(ih, "DROPFILESTARGET", "YES");
	}
	
//	IupSetCallback(ih, "_IUP_XY2POS_CB", (Icallback)cocoaTreeConvertXYToPos);
	
	iupdrvTreeUpdateMarkMode(ih);

	
	
	// Registering a custom type so we can do internal drag-and-drop (reordering)
	[outline_view registerForDraggedTypes:[NSArray arrayWithObjects:IUPCOCOA_OUTLINEVIEW_DRAGANDDROP_TYPE, nil]];

	
	
	return IUP_NOERROR;
}

static void cocoaTreeUnMapMethod(Ihandle* ih)
{
	id root_view = ih->handle;
	
	
	iupCocoaRemoveFromParent(ih);
	[root_view release];
	ih->handle = NULL;
}




void iupdrvTreeInitClass(Iclass* ic)
{
	/* Driver Dependent Class functions */
	ic->Map = cocoaTreeMapMethod;
	ic->UnMap = cocoaTreeUnMapMethod;
#if 0
	
	/* Visual */
	iupClassRegisterAttribute(ic, "BGCOLOR", NULL, cocoaTreeSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "TXTBGCOLOR", IUPAF_DEFAULT);
	iupClassRegisterAttribute(ic, "FGCOLOR", NULL, cocoaTreeSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "TXTFGCOLOR", IUPAF_DEFAULT);
#endif
	/* IupTree Attributes - GENERAL */
	iupClassRegisterAttribute(ic, "EXPANDALL", NULL, cocoaTreeSetExpandAllAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
#if 0
	iupClassRegisterAttribute(ic, "INDENTATION", cocoaTreeGetIndentationAttrib, cocoaTreeSetIndentationAttrib, NULL, NULL, IUPAF_DEFAULT);
	iupClassRegisterAttribute(ic, "SPACING", iupTreeGetSpacingAttrib, cocoaTreeSetSpacingAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NOT_MAPPED);
	iupClassRegisterAttribute(ic, "TOPITEM", NULL, cocoaTreeSetTopItemAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
#endif
	/* IupTree Attributes - IMAGES */
	iupClassRegisterAttributeId(ic, "IMAGE", NULL, cocoaTreeSetImageAttrib, IUPAF_IHANDLENAME|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
	iupClassRegisterAttributeId(ic, "IMAGEEXPANDED", NULL, cocoaTreeSetImageExpandedAttrib, IUPAF_IHANDLENAME|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
	
	iupClassRegisterAttribute(ic, "IMAGELEAF",            NULL, cocoaTreeSetImageLeafAttrib, IUPAF_SAMEASSYSTEM, "IMGLEAF", IUPAF_IHANDLENAME|IUPAF_NO_INHERIT);
	iupClassRegisterAttribute(ic, "IMAGEBRANCHCOLLAPSED", NULL, cocoaTreeSetImageBranchCollapsedAttrib, IUPAF_SAMEASSYSTEM, "IMGCOLLAPSED", IUPAF_IHANDLENAME|IUPAF_NO_INHERIT);
	iupClassRegisterAttribute(ic, "IMAGEBRANCHEXPANDED",  NULL, cocoaTreeSetImageBranchExpandedAttrib, IUPAF_SAMEASSYSTEM, "IMGEXPANDED", IUPAF_IHANDLENAME|IUPAF_NO_INHERIT);

#if 0
	/* IupTree Attributes - NODES */
	iupClassRegisterAttributeId(ic, "STATE",  cocoaTreeGetStateAttrib,  cocoaTreeSetStateAttrib, IUPAF_NO_INHERIT);
	iupClassRegisterAttributeId(ic, "DEPTH",  cocoaTreeGetDepthAttrib,  NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
	iupClassRegisterAttributeId(ic, "KIND",   cocoaTreeGetKindAttrib,   NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
	iupClassRegisterAttributeId(ic, "PARENT", cocoaTreeGetParentAttrib, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
	iupClassRegisterAttributeId(ic, "COLOR",  cocoaTreeGetColorAttrib,  cocoaTreeSetColorAttrib, IUPAF_NO_INHERIT);
#endif
	
	iupClassRegisterAttributeId(ic, "TITLE",  cocoaTreeGetTitleAttrib,  cocoaTreeSetTitleAttrib, IUPAF_NO_INHERIT);
	
#if 0
	iupClassRegisterAttributeId(ic, "TOGGLEVALUE", cocoaTreeGetToggleValueAttrib, cocoaTreeSetToggleValueAttrib, IUPAF_NO_INHERIT);
	iupClassRegisterAttributeId(ic, "TOGGLEVISIBLE", cocoaTreeGetToggleVisibleAttrib, cocoaTreeSetToggleVisibleAttrib, IUPAF_NO_INHERIT);
	
	/* Change the set method for GTK */
	iupClassRegisterReplaceAttribFunc(ic, "SHOWRENAME", NULL, cocoaTreeSetShowRenameAttrib);
	
	iupClassRegisterAttributeId(ic, "CHILDCOUNT", cocoaTreeGetChildCountAttrib, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
	iupClassRegisterAttributeId(ic, "TITLEFONT",  cocoaTreeGetTitleFontAttrib,  cocoaTreeSetTitleFontAttrib, IUPAF_NO_INHERIT);
	
	/* IupTree Attributes - MARKS */
	iupClassRegisterAttributeId(ic, "MARKED", cocoaTreeGetMarkedAttrib, cocoaTreeSetMarkedAttrib, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
	iupClassRegisterAttribute  (ic, "MARK",      NULL, cocoaTreeSetMarkAttrib,      NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
	iupClassRegisterAttribute  (ic, "STARTING",  NULL, cocoaTreeSetMarkStartAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
	iupClassRegisterAttribute  (ic, "MARKSTART", NULL, cocoaTreeSetMarkStartAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
	iupClassRegisterAttribute  (ic, "MARKEDNODES", cocoaTreeGetMarkedNodesAttrib, cocoaTreeSetMarkedNodesAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
	
	iupClassRegisterAttribute(ic, "MARKWHENTOGGLE", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
	
	iupClassRegisterAttribute  (ic, "VALUE", cocoaTreeGetValueAttrib, cocoaTreeSetValueAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
	
	/* IupTree Attributes - ACTION */
#endif
	iupClassRegisterAttributeId(ic, "DELNODE", NULL, cocoaTreeSetDelNodeAttrib, IUPAF_NOT_MAPPED|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
#if 0
	iupClassRegisterAttribute(ic, "RENAME", NULL, cocoaTreeSetRenameAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
#endif
	iupClassRegisterAttributeId(ic, "MOVENODE", NULL, cocoaTreeSetMoveNodeAttrib, IUPAF_NOT_MAPPED|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
#if 0
	iupClassRegisterAttributeId(ic, "COPYNODE", NULL, cocoaTreeSetCopyNodeAttrib, IUPAF_NOT_MAPPED|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
	
	/* IupTree Attributes - GTK Only */
	iupClassRegisterAttribute  (ic, "RUBBERBAND", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NO_INHERIT);
#endif

	helperLoadReplacementDefaultImages();
}
