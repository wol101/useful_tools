//
//  GaitSymSaverView.h
//  GaitSymSaver
//
//  Created by Bill Sellers on 30/03/2011.
//  Copyright (c) 2011, Bill Sellers. All rights reserved.
//

#import <ScreenSaver/ScreenSaver.h>

#import "GaitSymSaverOpenGLView.h"


@interface GaitSymSaverView : ScreenSaverView 
{
    IBOutlet id configSheet; 
    IBOutlet id drawFilledShapesOption; 
    IBOutlet id drawOutlinedShapesOption; 
    IBOutlet id drawBothOption;
    
    int numChildren;
    pid_t *pidList;
    
    GaitSymSaverOpenGLView *glView; 
    GLfloat rotation;
}

- (void)setUpOpenGL;

@end
