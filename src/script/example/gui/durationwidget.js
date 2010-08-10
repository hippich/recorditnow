RecordItNowImporter.importBinding("RecordItNow.MainWindow");
RecordItNowImporter.importBinding("qt.core");
RecordItNowImporter.importBinding("qt.gui");


function DurationWidget() {
  
    this.label = new QLabel();
    this.durationLabel = new QLabel();
    this.timer = new QTimer();
    this.widget = new QFrame();
    
    this.widget.objectName = "__DURATION_WIDGET__"
    this.widget.setFrameStyle(QFrame.StyledPanel|QFrame.Sunken);
    this.widget.lineWidth = 2;
    
    this.label.text = "Duration:";
  
    var layout = new QHBoxLayout();
    this.widget.setLayout(layout);

    layout.addWidget(this.label, 0, 0);
    layout.addWidget(this.durationLabel, 0, 0);

    MainWindow.addToolWidget(this.widget);

    MainWindow.recordStarted.connect(this, this.onRecordStarted);
    MainWindow.recordPaused.connect(this, this.onRecordPaused);
    MainWindow.recordResumed.connect(this, this.onRecordResumed);
    MainWindow.recordFinished.connect(this, this.onRecordFinished);
    
    this.timer.timeout.connect(this, this.onTimeout);
    this.timer.interval = 1000;
    
    this.durationLabel.setProperty("__DURATION__", -1);
    this.onTimeout();

}



DurationWidget.prototype.onRecordStarted = function(timer) {

    this.timer.start();
   
}


DurationWidget.prototype.onRecordPaused = function() {

    this.timer.stop();

}


DurationWidget.prototype.onRecordResumed = function() {

    this.timer.start();
    
}


DurationWidget.prototype.onRecordFinished = function() {

    this.timer.stop();
    
}


DurationWidget.prototype.onTimeout = function() {

    var duration = parseInt(this.durationLabel.property("__DURATION__"));
    duration = duration+1;
    
    
    var time = new QTime(0, 0, 0);
    time = time.addSecs(duration);
    
    this.durationLabel.setProperty("__DURATION__", duration);
    this.durationLabel.text = time.toString(Qt.TextDate);
    
}


DurationWidget.prototype.unload = function() {

    MainWindow.removeToolWidget(this.widget);
    this.widget.deleteLater();
    
}


