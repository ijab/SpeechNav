
$(document).ready(function() {
	try {
		/* 将tsWebProxyObject的signal 和 JavaScript slot 连接起来*/
		tsWebProxyObject.signal.connect(slot);
		
		tsWebProxyObject.TaskAdded.connect(taskSlot);
		//tsWebProxyObject.TaskInited.connect(taskSlot);
		//tsWebProxyObject.TaskStarted.connect(taskSlot);
		//tsWebProxyObject.TaskCompleted.connect(taskSlot);
		
		/* 当tsWebProxyObject发射信号时调用JavaScript的槽 */
		tsWebProxyObject.slotThatEmitsSignal();
		
		
		var object = {intValue: 1};
        /* This calls a slot which returns another object back */
        var returnedObject = tsWebProxyObject.slotThatReturns(object);
        /* This outputs "1 added bonus"*/
        alert(returnedObject.stringValue);
	}
	catch(e) {
		alert(e);
	}
});

/* slot函数将会输出 TSWebProxyObject has emited signal ？ times */
function slot(object) {
	var objectString = object.sender +
		" has emited signal " +
		object.signalsEmited +
		" times.";
	alert(objectString);
}

function taskSlot(object) {
    var objectString = "filename:" + object.filename + "-cmsid:" + 
                    object.cmsid + "-uuid:" + object.uuid;
    alert(objectString);
}

function submitThis(){
    var object = {intValue: 1};
    tsWebProxyObject.login(object);
    
    /* 当tsWebProxyObject发射信号时调用JavaScript的槽 */
	//tsWebProxyObject.slotThatEmitsSignal();
}

function upLoad(){

    var object = {"type" : "both", "filter": "Image Files(*.png *.jpg *.jpeg *.bmp *.gif);;All Files(*.*)", "singleSelection": false};
    var retV = tsWebProxyObject.select(object);
    
    for(var i = 0; i < retV.length; ++i)
    {
        alert(retV[i]);
    }
    
    var taskV = tsWebProxyObject.add({"filenames":retV});
    for( var o in taskV )
    {
        alert(o + ":" + retV[o]);
        var obj = new Object();
        obj[o] = retV[o];
        //tsWebProxyObject.remove(obj);
    }
}

function previewImg(){

    tsWebProxyObject.showUploadManager(false);
    tsWebProxyObject.showDownloadManager(false);
    tsWebProxyObject.loadProE("http://192.168.1.9:8080/fmcManager/170013.prt");
    return;
    var object = {"filter": "Image Files(*.png *.jpg *.jpeg *.bmp *.gif);;All Files(*.*)", "singleSelection": true};
    var val = tsWebProxyObject.previewImage(object);
    
    for( var o in val )
    {
        document.getElementById("preview_img").setAttribute("src", val[o]);
    }
}