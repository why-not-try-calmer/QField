import QtQuick 2.14
import QtQuick.Controls 2.14
import QtMultimedia 5.14

import Theme 1.0

Item{
  id : cameraItem
  signal finished(string path)
  signal canceled()

  property string currentPath

  anchors.fill: parent

  state: "PhotoCapture"

  states: [
    State {
      name: "PhotoCapture"
      StateChangeScript {
        script: {
          camera.captureMode = Camera.CaptureStillImage
        }
      }
    },
    State {
      name: "PhotoPreview"
    }
  ]

  Camera {
    id: camera

    position: Camera.BackFace

    imageCapture {
      onImageSaved: {
        currentPath  = path
      }
      onImageCaptured: {
        photoPreview.source = preview
        cameraItem.state = "PhotoPreview"
      }
    }
  }

  VideoOutput {
    anchors.fill: parent

    visible: cameraItem.state == "PhotoCapture"

    focus : visible
    source: camera

    autoOrientation: true

    MouseArea {
      anchors.fill: parent

      onClicked: {
        if (camera.lockStatus == Camera.Unlocked)
          camera.searchAndLock();
        else
          camera.unlock();
      }
    }


    QfToolButton {
      id: videoButtonClick
      visible: true

      anchors.right: parent.right
      anchors.verticalCenter: parent.verticalCenter

      round: true
      roundborder: true
      bgcolor: "grey"
      borderColor: Theme.mainColor

      onClicked: camera.imageCapture.captureToLocation(qgisProject.homePath+ '/DCIM/')
    }
  }

  Image {
    id: photoPreview

    visible: cameraItem.state == "PhotoPreview"

    anchors.fill: parent

    fillMode: Image.PreserveAspectFit
    smooth: true
    focus: visible

    QfToolButton {
      id: buttonok
      visible: true

      anchors.right: parent.right
      anchors.verticalCenter: parent.verticalCenter
      bgcolor: Theme.mainColor
      round: true

      iconSource: Theme.getThemeIcon("ic_save_white_24dp")

      onClicked: cameraItem.finished( currentPath )
    }

    QfToolButton {
      id: buttonnok
      visible: true

      anchors.right: parent.right
      anchors.top: parent.top
      bgcolor: Theme.mainColor
      round: true

      iconSource: Theme.getThemeIcon("ic_clear_white_24dp")
      onClicked: {
        platformUtilities.rmFile( currentPath )
        cameraItem.state = "PhotoCapture"
      }
    }
  }
}
