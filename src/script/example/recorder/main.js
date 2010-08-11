RecordItNowImporter.importBinding("qt.core");
RecordItNowImporter.importBinding("RecordItNow.Debug");
RecordItNowImporter.importBinding("qt.gui");  


QByteArray.prototype.toString = function()
{
  ts = new QTextStream( this, QIODevice.ReadOnly );
  return ts.readAll();
}


var process = 0;
load = function() {



}


unload = function() {

    if (process) {
        process.deleteLater();
    }
    
}


processFinished = function(exitCode, exitStatus) {

    print("ExitCode:", exitCode);
    RecordItNowRecorder.exit(0);

}


processOutput = function() {

    var output = process.readAllStandardOutput().toString(); 
    if (output.length < 1) {
        output = process.readAllStandardError().toString();
        if (output.length < 1) {
            return;
        }
    }
    output = output.replace(/^\s+/, '').replace (/\s+$/, ''); // trimmed

    var lines = output.split("\n");
    for (var i = 0; i < lines.length; i++) {
        var line = lines[i];
        RecordItNowDebug.debug(line);

       if (line == "Capturing!") {
          RecordItNowDebug.debug("STATUS");
          RecordItNowRecorder.sendStatus("Capturing!");
       } else if (line.charAt(0) == "[") {
           RecordItNowRecorder.setRecorderState(2);
           for (var c = 0; c < line.length; c++) {
               if (line.charAt(0) == "[") {
                   line = line.substr(c+1, line.lenght);
                   var percent = line.substr(0, line.indexOf("%"));
                   percent = "Encode: " + percent + "%";
                   RecordItNowRecorder.sendStatus(percent);
               }
           }
        }

    }

}


record = function() {

    var args = new Array();
    args.push("--fps");
    args.push(FPS);
    if (Window != -1) {
        args.push("--windowid");
        args.push(Window);
    } else {
        args.push("-x");
        args.push(X);
        args.push("-y");
        args.push(Y);
        args.push("--width");
        args.push(Width);
        args.push("--height");
    }

    args.push("--workdir");
    args.push(WorkDir);
    args.push("-o");
    args.push(Outputfile); 

    if (!SoundEnabled) {
        args.push("--no-sound");
    }

    print("Args:", args);


    process = new QProcess();
    process.setProcessChannelMode(process.MergedChannels);
   

    process['finished(int,QProcess::ExitStatus)'].connect(processFinished);
    process.readyReadStandardOutput.connect(processOutput);
    process.readyReadStandardError.connect(processOutput);
    process.start("recordmydesktop", args);

}


stop = function() {

    process.terminate();

}


pause = function() {

    

}


isVideoRecorder = function()  {

    return true;

}
