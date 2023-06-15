//
//  GaitSymSaverView.m
//  GaitSymSaver
//
//  Created by Bill Sellers on 30/03/2011.
//  Copyright (c) 2011, Bill Sellers. All rights reserved.
//

#import "GaitSymSaverView.h"

#import <OpenGL/gl.h>
#import <OpenGL/glu.h>
#import <unistd.h>

@implementation GaitSymSaverView

static NSString * const MyModuleName = @"com.animalsimulation.GaitSymSaver";

- (id)initWithFrame:(NSRect)frame isPreview:(BOOL)isPreview
{
    self = [super initWithFrame:frame isPreview:isPreview];
    if (self) 
    {
        NSOpenGLPixelFormatAttribute attributes[] = 
        { 
            NSOpenGLPFAAccelerated, 
            NSOpenGLPFADepthSize, 16, 
            NSOpenGLPFAMinimumPolicy, 
            NSOpenGLPFAClosestPolicy,
            0 
        }; 
        NSOpenGLPixelFormat *format = [[[NSOpenGLPixelFormat alloc] initWithAttributes:attributes] autorelease];
        
        glView = [[GaitSymSaverOpenGLView alloc] initWithFrame:NSZeroRect pixelFormat:format];
        if (!glView) 
        {
            NSLog( @"Couldn't initialize OpenGL view." ); 
            [self autorelease]; 
            return nil;
        }
        [self addSubview:glView]; 
        [self setUpOpenGL];
        
        ScreenSaverDefaults *defaults; 
        defaults = [ScreenSaverDefaults defaultsForModuleWithName:MyModuleName];
        // Register our default values 
        [defaults registerDefaults:[NSDictionary dictionaryWithObjectsAndKeys:
                                    @"NO", @"DrawFilledShapes", 
                                    @"NO", @"DrawOutlinedShapes", 
                                    @"YES", @"DrawBoth", 
                                    nil]];
        
        [self setAnimationTimeInterval:1/30.0];
        
        NSUInteger processorCount = [[NSProcessInfo processInfo] processorCount];
        numChildren = processorCount - 1; // because I want any unused cores to run the non-graphic versions
    }
    return self;
}

- (void)dealloc 
{
    [glView removeFromSuperview]; 
    [glView release]; 
    [super dealloc];
}

- (void)startAnimation
{
    NSBundle *bundle = [NSBundle bundleForClass:[GaitSymSaverView class]];
    NSString *pathToExecutable = [bundle pathForResource:@"gaitsym_tcp" ofType:@""];
    NSLog(@"GaitSymSaverView startAnimation gaitsym_tcp path = %@\n", pathToExecutable);
    
    // fork to run n copies of GaitSym
    
    pidList = malloc(numChildren * sizeof(pid_t));
    
    for (int i = 0; i < numChildren; i++) 
    {
        pid_t pid = fork();
        if (pid == -1) 
        {
            return; /* error - just return */
        } 
        else 
        {
            if (pid == 0) 
            {
                /* child - so exec gaitsym */
                
                execl([pathToExecutable	UTF8String], "gaitsym_tcp", "--Server", "130.88.95.159:8086", "-q", (char *)0);
                break;
            } 
            else 
            {
                /* parent - so update list of child PIDs */
                pidList[i] = pid;
            }
        }
    }
                         
    [super startAnimation];
}

- (void)stopAnimation
{
    // kill the PIDs of all the child processes
    
    for (int i = 0; i < numChildren; i++)
    {
        kill(pidList[i], SIGKILL);
    }
    free(pidList);
    
    [super stopAnimation];
}

- (void)drawRect:(NSRect)rect
{
    [super drawRect:rect];
    
    [[glView openGLContext] makeCurrentContext];
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
    glLoadIdentity();
    
    glTranslatef( -1.5f, 0.0f, -6.0f);
    glRotatef( rotation, 0.0f, 1.0f, 0.0f);
    
    glBegin( GL_TRIANGLES ); 
    {
        glColor3f( 1.0f, 0.0f, 0.0f ); 
        glVertex3f( 0.0f, 1.0f, 0.0f ); 
        glColor3f( 0.0f, 1.0f, 0.0f ); 
        glVertex3f( -1.0f, -1.0f, 1.0f ); 
        glColor3f( 0.0f, 0.0f, 1.0f ); 
        glVertex3f( 1.0f, -1.0f, 1.0f );
        
        glColor3f( 1.0f, 0.0f, 0.0f ); 
        glVertex3f( 0.0f, 1.0f, 0.0f ); 
        glColor3f( 0.0f, 0.0f, 1.0f ); 
        glVertex3f( 1.0f, -1.0f, 1.0f ); 
        glColor3f( 0.0f, 1.0f, 0.0f ); 
        glVertex3f( 1.0f, -1.0f, -1.0f );
        
        glColor3f( 1.0f, 0.0f, 0.0f ); 
        glVertex3f( 0.0f, 1.0f, 0.0f ); 
        glColor3f( 0.0f, 1.0f, 0.0f ); 
        glVertex3f( 1.0f, -1.0f, -1.0f ); 
        glColor3f( 0.0f, 0.0f, 1.0f ); 
        glVertex3f( -1.0f, -1.0f, -1.0f );
        
        glColor3f( 1.0f, 0.0f, 0.0f ); 
        glVertex3f( 0.0f, 1.0f, 0.0f ); 
        glColor3f( 0.0f, 0.0f, 1.0f ); 
        glVertex3f( -1.0f, -1.0f, -1.0f ); 
        glColor3f( 0.0f, 1.0f, 0.0f ); 
        glVertex3f( -1.0f, -1.0f, 1.0f );
    } 
    
    glEnd();
    glFlush();
}

- (void)animateOneFrame
{
    // Adjust our state 
    rotation += 0.2f;
    
    // Redraw 
    [self setNeedsDisplay:YES];
    
    /*
    NSBezierPath *path; 
    NSRect rect; 
    NSSize size;
    NSColor *color; 
    float red, green, blue, alpha; 
    int shapeType;
    ScreenSaverDefaults *defaults;
    
    size = [self bounds].size;
    
    // Calculate random width and height 
    rect.size = NSMakeSize( SSRandomFloatBetween( size.width / 100.0,
                                                 size.width / 10.0 ), 
                           SSRandomFloatBetween( size.height / 100.0,
                                                 size.height / 10.0 ));
    
    // Calculate random origin point 
    rect.origin = SSRandomPointForSizeWithinRect( rect.size, [self bounds] );
    
    // Decide what kind of shape to draw 
    shapeType = SSRandomIntBetween( 0, 2 );
    switch (shapeType) 
    {
        case 0: // rect 
            path = [NSBezierPath bezierPathWithRect:rect]; 
            break;
        case 1: // oval 
            path = [NSBezierPath bezierPathWithOvalInRect:rect]; 
            break;
        case 2: // arc
        default: 
        {
            float startAngle, endAngle, radius; 
            NSPoint point;
            startAngle = SSRandomFloatBetween( 0.0, 360.0 ); 
            endAngle = SSRandomFloatBetween( startAngle, 360.0 + startAngle );
            // Use the smallest value for the radius (either width or height) 
            radius = rect.size.width <= rect.size.height ? rect.size.width / 2 : rect.size.height / 2;
            // Calculate our center point 
            point = NSMakePoint( rect.origin.x + rect.size.width / 2, rect.origin.y + rect.size.height / 2 );
            // Construct the path 
            path = [NSBezierPath bezierPath];
            [path appendBezierPathWithArcWithCenter: point 
                                             radius: radius
                                         startAngle: startAngle 
                                           endAngle: endAngle
                                          clockwise: SSRandomIntBetween( 0, 1 )];
        } 
            break;
    }
    
    // Calculate a random color 
    red = SSRandomFloatBetween( 0.0, 255.0 ) / 255.0; 
    green = SSRandomFloatBetween( 0.0, 255.0 ) / 255.0; 
    blue = SSRandomFloatBetween( 0.0, 255.0 ) / 255.0; 
    alpha = SSRandomFloatBetween( 0.0, 255.0 ) / 255.0;
    color = [NSColor colorWithCalibratedRed:red 
                                      green:green
                                       blue:blue 
                                      alpha:alpha];
    
    [color set];
    
    // And finally draw it 
    defaults = [ScreenSaverDefaults defaultsForModuleWithName:MyModuleName];
    if ([defaults boolForKey:@"DrawBoth"]) 
    {
        if (SSRandomIntBetween( 0, 1 ) == 0) 
            [path fill];
        else 
            [path stroke];
    } 
    else if ([defaults boolForKey:@"DrawFilledShapes"])
        [path fill]; 
    else
        [path stroke];
     */
                                         
}

- (BOOL)hasConfigureSheet
{
    return YES;
}

- (NSWindow*)configureSheet
{
    ScreenSaverDefaults *defaults; 
    defaults = [ScreenSaverDefaults defaultsForModuleWithName:MyModuleName];
    
    if (!configSheet) 
    {
        if (![NSBundle loadNibNamed:@"ConfigureSheet" owner:self]) 
        {
            NSLog( @"Failed to load configure sheet." ); 
            NSBeep();
        }
    } 
    
    [drawFilledShapesOption setState:[defaults boolForKey:@"DrawFilledShapes"]];
    [drawOutlinedShapesOption setState:[defaults boolForKey:@"DrawOutlinedShapes"]];
    [drawBothOption setState:[defaults boolForKey:@"DrawBoth"]];
    
    return configSheet;
}

- (IBAction)cancelClick:(id)sender
{ 
    [[NSApplication sharedApplication] endSheet:configSheet];
}

- (IBAction) okClick: (id)sender 
{
    ScreenSaverDefaults *defaults; 
    defaults = [ScreenSaverDefaults defaultsForModuleWithName:MyModuleName];
    // Update our defaults 
    [defaults setBool:[drawFilledShapesOption state]
               forKey:@"DrawFilledShapes"]; 
    [defaults setBool:[drawOutlinedShapesOption state]
               forKey:@"DrawOutlinedShapes"]; 
    [defaults setBool:[drawBothOption state]
               forKey:@"DrawBoth"];
    
    // Save the settings to disk 
    [defaults synchronize];
    
    // Close the sheet 
    [[NSApplication sharedApplication] endSheet:configSheet];
}

- (void)setUpOpenGL 
{
    [[glView openGLContext] makeCurrentContext];
    glShadeModel( GL_SMOOTH ); 
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f ); 
    glClearDepth( 1.0f ); 
    glEnable( GL_DEPTH_TEST ); 
    glDepthFunc( GL_LEQUAL ); 
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
    rotation = 0.0f; 
}


- (void)setFrameSize:(NSSize)newSize 
{
    [super setFrameSize:newSize]; 
    [glView setFrameSize:newSize];
    
    [[glView openGLContext] makeCurrentContext];
    // Reshape 
    glViewport( 0, 0, (GLsizei)newSize.width, (GLsizei)newSize.height ); 
    glMatrixMode( GL_PROJECTION ); glLoadIdentity(); 
    gluPerspective( 45.0f, (GLfloat)newSize.width / (GLfloat)newSize.height, 0.1f, 100.0f ); 
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity(); 
    
    [[glView openGLContext] update];
}

@end
