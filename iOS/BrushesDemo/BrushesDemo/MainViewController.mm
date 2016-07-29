//
//  MainViewController.m
//  AppleCoder-OpenGLES-00
//
//  Created by CharlyZhang on 14-11-6.
//
//

#import "MainViewController.h"
#import "HYBrushCore.h"
#import "CanvasView.h"
#import "LayersTableViewController.h"

#import <CoreGraphics/CoreGraphics.h>

#define BOTTOM_OFFSET   108

@interface MainViewController ()<UIPickerViewDelegate, UIPickerViewDataSource, UITextFieldDelegate>
{
    NSArray *brushesName;
    
    UIPopoverController *popoverController_;
    LayersTableViewController *layerController_;
    NSInteger currentLayerIdx;
    HYBrushCore *coreInstance;
}
@property (retain, nonatomic) IBOutlet UIPickerView *brushesSelectView;
@property (retain, nonatomic) IBOutlet UIButton *undoButton;
@property (retain, nonatomic) IBOutlet UIButton *redoButton;
@property (retain, nonatomic) IBOutlet UITextField *itentityTF;
@property (retain, nonatomic) IBOutlet UITextField *angleTF;
@property (retain, nonatomic) IBOutlet UITextField *spacingTF;
@property (retain, nonatomic) IBOutlet UITextField *jitterTF;
@property (retain, nonatomic) IBOutlet UITextField *scatterTF;
@property (retain, nonatomic) IBOutlet UITextField *bDensityTF;
@property (retain, nonatomic) IBOutlet UITextField *weightTF;
@property (retain, nonatomic) IBOutlet UITextField *bSizeTF;
@property (retain, nonatomic) IBOutlet UITextField *DangleTF;
@property (retain, nonatomic) IBOutlet UITextField *DweightTF;
@property (retain, nonatomic) IBOutlet UITextField *DintentityTF;

@property (weak, nonatomic) IBOutlet UISlider *brushSizeSlider;
@property (weak, nonatomic) IBOutlet UISlider *bDensitySlider;
@property (weak, nonatomic) IBOutlet UISlider *bSizeSlider;
@property (weak, nonatomic) IBOutlet UISlider *DangleSlider;
@property (weak, nonatomic) IBOutlet UISlider *DweightSlider;
@property (weak, nonatomic) IBOutlet UISlider *DintentitySlider;
@property (weak, nonatomic) IBOutlet UISlider *jitterSlider;
@property (weak, nonatomic) IBOutlet UISlider *scatterSlider;
@property (weak, nonatomic) IBOutlet UISlider *intentitySlider;
@property (weak, nonatomic) IBOutlet UISlider *angleSlider;
@property (weak, nonatomic) IBOutlet UISlider *spacingSlider;

@property (assign, nonatomic) UISwitch *colorFillSwitcher;
@property (assign, nonatomic) NSInteger layersNumber;

@property (strong, nonatomic) UIColor *activeColor;
@property (assign, nonatomic) float brushSize;
@property (assign, nonatomic) float spacing;
@property (assign, nonatomic) float angle;
@property (assign, nonatomic) float intentity;
@property (assign, nonatomic) float jitter;
@property (assign, nonatomic) float scatter;
@property (assign, nonatomic) float dynamicAngle;
@property (assign, nonatomic) float dynamicWeight;
@property (assign, nonatomic) float dynamicIntentity;
@property (assign, nonatomic) float bristleDensity;
@property (assign, nonatomic) float bristleSize;

@end

@implementation MainViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor lightGrayColor];


    NSLog(@"sandbox path is:%@",NSHomeDirectory());
    
    CGSize size = [UIScreen mainScreen].bounds.size;
    
    coreInstance = [HYBrushCore sharedInstance];
    CanvasView *canvasView = [coreInstance initializeWithWidth:size.width
                                                        Height:size.height-BOTTOM_OFFSET
                                                   ScreenScale:[UIScreen mainScreen].scale
                                                 GLSLDirectory:[[[NSBundle mainBundle]bundlePath] stringByAppendingString:@"/BrushesCoreResources.bundle/glsl/"]];

    [self.view insertSubview:canvasView atIndex:0];
    
    UIImage *brushImage = [UIImage imageNamed:@"BrushesCoreResources.bundle/stamp_images/watercolorPen.png"];
    [coreInstance setCurrentBrushStamp:brushImage];
    
    self.brushSize = 10.0f;
    self.activeColor = [UIColor blackColor];
    self.spacing = coreInstance.spacing4ActiveBrush;
    self.angle = coreInstance.angle4ActiveBrush;
    self.intentity = coreInstance.intentity4ActiveBrush;
    self.jitter = coreInstance.jitter4ActiveBrush;
    self.scatter = coreInstance.scatter4ActiveBrush;
    self.dynamicAngle = coreInstance.dynamicAngle4ActiveBrush;
    self.dynamicWeight = coreInstance.dynamicWeight4ActiveBrush;
    self.dynamicIntentity = coreInstance.dynamicIntensity4ActiveBrush;
    self.bristleSize = coreInstance.bristleSize4ActiveBrush;
    self.bristleDensity = coreInstance.bristleDentity4ActiveBrush;
    
//    self.colorFillSwitcher = nil;
//    self.layersNumber = [coreInstance getLayersNumber];
}

- (BOOL)shouldAutorotate {
    return YES;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Properties
- (void) setActiveColor:(UIColor*)color
{
    _activeColor = color;
    [coreInstance setActiveStateColor:color];
}

- (void) setBrushSize:(float)size
{
    _brushSize = size;
    [coreInstance setActiveBrushSize:size];
    self.weightTF.text = [NSString stringWithFormat:@"大小:%0.2f",size];
    self.brushSizeSlider.value = size;
}

- (void) setSpacing:(float)spacing
{
    _spacing = spacing;
    [coreInstance setSpacing4ActiveBrush:spacing];
    self.spacingTF.text = [NSString stringWithFormat:@"spacing:%0.2f",spacing];
    self.spacingSlider.value = spacing;
}

- (void) setAngle:(float)angle
{
    _angle = angle;
    [coreInstance setAngle4ActiveBrush:angle];
    self.angleTF.text = [NSString stringWithFormat:@"angle:%0.2f",angle];
    self.angleSlider.value = angle;
}

- (void) setIntentity:(float)intentity
{
    _intentity = intentity;
    [coreInstance setIntentity4ActiveBrush:intentity];
    self.itentityTF.text = [NSString stringWithFormat:@"intentity:%0.2f",intentity];
    self.intentitySlider.value = intentity;
}

- (void) setJitter:(float)jitter
{
    _jitter = jitter;
    [coreInstance setJitter4ActiveBrush:jitter];
    self.jitterTF.text = [NSString stringWithFormat:@"jitter:%0.2f",jitter];
    self.jitterSlider.value = jitter;
}

- (void) setScatter:(float)scatter
{
    _scatter = scatter;
    [coreInstance setScatter4ActiveBrush:scatter];
    self.scatterTF.text = [NSString stringWithFormat:@"scatter:%0.2f",scatter];
    self.scatterSlider.value = scatter;
}

- (void) setDynamicAngle:(float)dynamicAngle
{
    _dynamicAngle = dynamicAngle;
    [coreInstance setDynamicAngle4ActiveBrush:dynamicAngle];
    self.DangleTF.text = [NSString stringWithFormat:@"Dangle:%0.2f",dynamicAngle];
    self.DangleSlider.value = dynamicAngle;
}

- (void) setDynamicWeight:(float)dynamicWeight
{
    _dynamicWeight = dynamicWeight;
    [coreInstance setDynamicWeight4ActiveBrush:dynamicWeight];
    self.DweightTF.text = [NSString stringWithFormat:@"Dangle:%0.2f",dynamicWeight];
    self.DweightSlider.value = dynamicWeight;
}

- (void) setDynamicIntentity:(float)dynamicIntentity
{
    _dynamicIntentity = dynamicIntentity;
    [coreInstance setDynamicIntensity4ActiveBrush:dynamicIntentity];
    self.DintentityTF.text = [NSString stringWithFormat:@"Dintentity:%0.2f",dynamicIntentity];
    self.DintentitySlider.value = dynamicIntentity;
}

- (void) setBristleDensity:(float)bristleDensity
{
    _bristleDensity = bristleDensity;
    [coreInstance setBristleDentity4ActiveBrush:bristleDensity];
    self.bDensityTF.text = [NSString stringWithFormat:@"bDensity:%0.2f",bristleDensity];
    self.bDensitySlider.value = bristleDensity;
}

- (void) setBristleSize:(float)bristleSize
{
    _bristleSize = bristleSize;
    [coreInstance setBristleSize4ActiveBrush:bristleSize];
    self.bSizeTF.text = [NSString stringWithFormat:@"bSize:%0.2f",bristleSize];
    self.bSizeSlider.value = bristleSize;
}

#pragma mark - Actions

- (IBAction)close:(UIButton *)sender {
    CanvasView *canvasView = [[self.view subviews] objectAtIndex:0];
    [canvasView removeFromSuperview];
    [coreInstance restoreCore];
}

- (IBAction)clearButton:(UIButton *)sender {
    NSInteger activeLayerIdx = [coreInstance getActiveLayerIndex];
    [coreInstance clearLayer:activeLayerIdx];
}

- (IBAction)adjustProperty:(UISlider *)sender {
    switch (sender.tag) {
        case 0: ///< intentity
            self.intentity = sender.value;
            break;
        case 1: ///< angle
            self.angle = sender.value;
            break;
        case 2: ///< spacing
            self.spacing = sender.value;
            break;
        case 3: ///< dynamic itentity
            self.dynamicIntentity = sender.value;
            break;
        case 4: ///< jitter
            self.jitter = sender.value;
            break;
        case 5: ///< scatter
            self.scatter = sender.value;
            break;
        case 6: ///< dynamic weight
            self.dynamicWeight = sender.value;
            break;
        case 7: ///< bristle dentity
            self.bristleDensity = sender.value;
            break;
        case 8: ///< bristle size
            self.bristleSize = sender.value;
            break;
        case 9: ///< dynamic angle
            self.dynamicAngle = sender.value;
            break;
        default:
            break;
    }
}

- (IBAction)undoButtonAction:(UIButton *)sender {
    NSInteger activeLayerIdx = [coreInstance getActiveLayerIndex];
    [coreInstance undoPaintingOfLayer:activeLayerIdx];
    self.undoButton.enabled = false;
    self.redoButton.enabled = true;
}

- (IBAction)redoButtonAction:(UIButton *)sender {
    NSInteger activeLayerIdx = [coreInstance getActiveLayerIndex];
    [coreInstance redoPaintingOfLayer:activeLayerIdx];
    self.undoButton.enabled = true;
    self.redoButton.enabled = false;
}


- (IBAction)sizeSlider:(UISlider *)sender {
    NSLog(@"size of slider : %f",sender.value);
    self.brushSize = sender.value;
}
- (IBAction)redSlider:(UISlider *)sender {
//    float red = sender.value;
//    UIColor *color = [UIColor ];
//    [coreInstance setActiveStateColor:color];
}
- (IBAction)greenSlider:(UISlider *)sender {
//    green = sender.value;
//    [self updatePaintColor];
}
- (IBAction)blueSlider:(UISlider *)sender {
////    blue = sender.value;
////    [self updatePaintColor];
}
- (IBAction)modeSegmentedCtrl:(UISegmentedControl *)sender {
//    NSLog(@"mode segmented controller is:%d",sender.selectedSegmentIndex);
//    if (sender.selectedSegmentIndex == 0) {
//        CZActiveState::getInstance()->setEraseMode(false);
//    }
//    else {
//        CZActiveState::getInstance()->setEraseMode(true);
//    }
}


- (IBAction)layerButton:(UIButton *)sender {
        
//    if (!layerController_) {
//        layerController_ = [[LayersTableViewController alloc]init];
////        layerController_ = [[LayersTableViewController alloc] initWithNibName:@"Layers" bundle:nil];
//        layerController_.painting = painting;
////        layerController_.delegate = self;
//        
//        popoverController_ = [[UIPopoverController alloc] initWithContentViewController:layerController_];
//    }
//    
//    [popoverController_ presentPopoverFromRect:CGRectInset(((UIView *) sender).bounds, 10, 10)
//                                        inView:sender
//                      permittedArrowDirections:(UIPopoverArrowDirectionUp | UIPopoverArrowDirectionDown)
//                                      animated:YES];
}

- (IBAction)controlLayerStepper:(UIStepper *)sender {
//    NSLog(@"layer number is %f",sender.value);
//    
//    if (sender.value > self.layersNumber) {
//        painting->addNewLayer();
//    }
//    else if (sender.value < self.layersNumber) {
//        painting->deleteActiveLayer();
//    }
//    
//    self.layersNumber = sender.value;
//    canvas->drawView();
}


#pragma mark - BrushCore Related Methods

- (void)selectBrush:(NSUInteger) brushIdx {
//    NSLog(@"selected brush index: %u",brushIdx);
//    CZActiveState::getInstance()->setActiveBrush(brushIdx);
}

#pragma mark - UIPickerViewDelegate Methods
- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView{
    return 1;
}

// returns the # of rows in each component..
- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component{
//    return CZActiveState::getInstance()->getBrushesNumber();
    return 0;
}

-(NSString*) pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component{
    return [brushesName objectAtIndex:row];
}

- (void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component {
    [self selectBrush:row];
}

@end
