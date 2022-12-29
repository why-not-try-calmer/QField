import QtQuick 2.14

import org.qfield 1.0

Item {
  id: geometryHighlighter
  property alias geometryWrapper: geometryRenderer.geometryWrapper
  property int duration: 3000

  Connections {
    target: geometryRenderer.geometryWrapper

    function onQgsGeometryChanged() {
      timer.restart()
    }
  }

  GeometryRenderer {
    id: geometryRenderer
    mapSettings: mapCanvas.mapSettings
  }

  Timer {
    id: timer
    interval: geometryHighlighter.duration
    running: false
    repeat: false
    onTriggered: geometryHighlighter.geometryWrapper.clear()
  }
}
