//
//  ViewController.swift
//  Brush_swift
//
//  Created by CharlyZhang on 15-7-2.
//  Copyright (c) 2015年 Founder. All rights reserved.
//

import UIKit

let BOTTOM_OFFSET = 108 as Float

class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        
        let coreInstance = HYBrushCore.sharedInstance();
        let width = Float(UIScreen.mainScreen().bounds.size.width);
        let height = Float(UIScreen.mainScreen().bounds.size.height);
        let canvasView = coreInstance.initializeWithWidth(width, height: height-BOTTOM_OFFSET, screenScale: Float(UIScreen.mainScreen().scale), GLSLDirectory: NSBundle.mainBundle().bundlePath.stringByAppendingString("/BrushesCoreResources.bundle/glsl/"));
        
        self.view.insertSubview(canvasView, atIndex:0);
        
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

