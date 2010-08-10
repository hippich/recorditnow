if (!RecordItNowImporter.include("durationwidget.js")) {
    throw "Could not import ReplayGainer.js - aborting";
}


var widget = 0;
load = function() {

    widget = new DurationWidget;
}


unload = function() {
    
    widget.unload();
    
}


