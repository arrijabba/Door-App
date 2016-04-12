//
//  ViewController.swift
//  Door
//
//  Created by arri on 4/6/16.
//  Copyright © 2016 arri. All rights reserved.
//

import UIKit

class ViewController: UIViewController,UITextFieldDelegate {
    
    func textFieldShouldReturn(textField: UITextField) -> Bool {
        self.view.endEditing(true);
        return false;
    }
    

    @IBOutlet weak var statusLabl: UILabel!
    @IBOutlet weak var ipField: UITextField!
    
    @IBOutlet weak var bellLabl: UILabel!
    
    var client:YSocket = TCPClient();
    
    var connected:Bool = false;
    

    @IBAction func doorBtn(sender: UIButton) {
        
        if(statusLabl.text == "OPEN")
        {
            return;
        }
        
        statusLabl.text = "sending...";
        
        let openData:[UInt8] = [0x00];
        
        let (success, errmsg) = (client as! TCPClient).send(data: openData);
        
        if(success)
        {
            statusLabl.text = "OPEN";
        }
        else
        {
            statusLabl.text = errmsg;
        }
        
        NSTimer.scheduledTimerWithTimeInterval(NSTimeInterval(10), target: self, selector: "closeDoor", userInfo: nil, repeats: false);
    }
    
    func closeDoor(){
        
        statusLabl.text = "CLOSED";

    }
    
    func checkDoor(){
        
        let data = (client as! TCPClient).read(4, timeout: 1);
        
        if(data?.count > 1)
        {
            if let str = NSString(bytes: data!,length:(data?.count)!, encoding: NSUTF8StringEncoding){
                bellLabl.text = str as String;
            }
        }
        else
        {
            bellLabl.text = "Idle";
        }
        
    }
    
    
    @IBAction func readBtn(sender: UIButton) {
        
        let data = (client as! TCPClient).read(10,timeout: 10);
        
        if(data?.count > 0)
        {
            if let str = NSString(bytes: data!,length:(data?.count)!, encoding: NSUTF8StringEncoding){
                statusLabl.text = str as String;
            }
        }
        
    }
    
    @IBAction func connectBtn(sender: UIButton) {
        
        if(connected)
        {
            return;
        }
        
        statusLabl.text = "Connecting...";
        
        //start connect logic
        //success: update status to connected
        //failed: update status to failed to connect
        
        //schedule a check of latest status on buffer
        //add to log? scroll view?
        
        client = TCPClient(addr: ipField.text!, port: 80);
        
        let (success, errmsg) = (client as! TCPClient).connect(timeout: 10);
        
        if(success)
        {
            statusLabl.text = "Connected..";
            connected = true;
        }
        else
        {
            statusLabl.text = errmsg;
        }
        
        let data = (client as! TCPClient).read(15);
        
        if(data?.count > 0)
        {
            if let str = NSString(bytes: data!,length:(data?.count)!, encoding: NSUTF8StringEncoding){
                statusLabl.text = str as String;
                
                NSTimer.scheduledTimerWithTimeInterval(NSTimeInterval(3), target: self, selector: "checkDoor", userInfo: nil, repeats: true);

            }
        }
        
    }
    

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        
        self.ipField.delegate = self;
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

