import QtQuick 2.14
import QtQuick.Controls 2.14

import org.qfield 1.0
import Theme 1.0

import "."

EditorWidgetBase {
  id: valueMap

  anchors {
    left: parent.left
    right: parent.right
  }

  property var currentKeyValue: value
  // Workaround to get a signal when the value has changed
  onCurrentKeyValueChanged: {
    comboBox.currentIndex = comboBox.model.keyToIndex(currentKeyValue)
  }

  height: childrenRect.height
  enabled: isEnabled


  ComboBox {
    id: comboBox
    anchors { left: parent.left; right: parent.right }
    font: Theme.defaultFont
    popup.font: Theme.defaultFont

    currentIndex: model.keyToIndex(value)

    model: ValueMapModel {
      id: listModel

      onMapChanged: {
        comboBox.currentIndex = keyToIndex(valueMap.currentKeyValue)
      }
    }

    Component.onCompleted:
    {
      comboBox.popup.z = 10000 // 1000s are embedded feature forms, use a higher value to insure popups always show above embedded feature formes
      model.valueMap = config['map']
    }

    textRole: 'value'

    onCurrentTextChanged: {
      var key = model.keyForValue(currentText)
      valueChangeRequested(key, false)
    }

    MouseArea {
      anchors.fill: parent
      propagateComposedEvents: true

      onClicked: mouse.accepted = false
      onPressed: { forceActiveFocus(); mouse.accepted = false; }
      onReleased: mouse.accepted = false;
      onDoubleClicked: mouse.accepted = false;
      onPositionChanged: mouse.accepted = false;
      onPressAndHold: mouse.accepted = false;
    }

    contentItem: Text {
        leftPadding: enabled ? 5 : 0

        text: comboBox.displayText
        font: comboBox.font
        color: enabled ? 'black' : 'gray'
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        elide: Text.ElideRight
    }

    background: Item {
      implicitWidth: 120
      implicitHeight: 36

      Rectangle {
        visible: !enabled
        y: comboBox.height - 12
        width: comboBox.width
        height: comboBox.activeFocus ? 2: 1
        color: comboBox.activeFocus ? Theme.accentColor : Theme.accentLightColor
      }

      Rectangle {
        visible: enabled
        anchors.fill: parent
        id: backgroundRect
        border.color: comboBox.pressed ? Theme.accentColor : Theme.accentLightColor
        border.width: comboBox.visualFocus ? 2 : 1
        color: Theme.lightGray
        radius: 2
      }
    }
  }
}
