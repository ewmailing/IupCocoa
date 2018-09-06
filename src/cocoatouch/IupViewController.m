//
//  IupLaunchViewController.m
//  iup
//
//  Created by Eric Wing on 12/22/16.
//  Copyright © 2016 Tecgraf, PUC-Rio, Brazil. All rights reserved.
//

#import "IupViewController.h"
#include "iupcocoatouch_drv.h"
#include "iup_object.h"

@interface IupViewController ()

@end

@implementation IupViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

- (void) viewWillTransitionToSize:(CGSize)to_size withTransitionCoordinator:(id<UIViewControllerTransitionCoordinator>)transition_coordinator
{
	[super viewWillTransitionToSize:to_size withTransitionCoordinator:transition_coordinator];

    [transition_coordinator animateAlongsideTransition:^(id<UIViewControllerTransitionCoordinatorContext> _Nonnull context)
		{
			// Code here will execute before the rotation begins.
			// Equivalent to placing it in the deprecated method -[willRotateToInterfaceOrientation:duration:]

			// Place code here to perform animations during the rotation.
			// You can pass nil for this closure if not necessary.
			Ihandle* ih = [self ihandle];

			ih->naturalwidth = to_size.width;
			ih->naturalheight = to_size.height;
		
			iupCocoaTouchDoResize(ih, to_size, self);
		}
	completion:^(id<UIViewControllerTransitionCoordinatorContext> _Nonnull context)
		{
			// Code here will execute after the rotation has finished.
			// Equivalent to placing it in the deprecated method -[didRotateFromInterfaceOrientation:]
		}
    ];
}

@end
