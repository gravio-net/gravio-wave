import QtQuick 2.9
import QtQuick.Dialogs 1.0
import Qt.labs.platform 1.0

FileDialog
{
    id: fileDialog
    title: "Please choose picture"
    folder: StandardPaths.standardLocations(StandardPaths.PicturesLocation)[0]
    //folder: shortcuts.pictures
    //selectExisting: true
    //selectMultiple: false
    //selectFolder: false
    nameFilters: [ "Image files (*.jpg *.png)", "All files (*)" ]

    property variant caller;

    function execute(component)
    {
        caller = component;
        this.open();
    }

    onAccepted:
    {
        caller.applyChoise(fileDialog.currentFile);
        fileDialog.close();
    }

    onRejected:
    {
        fileDialog.close();
    }
}

