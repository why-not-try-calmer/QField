/***************************************************************************
                            FeatureListForm.qml
                              -------------------
              begin                : 10.12.2014
              copyright            : (C) 2014 by Matthias Kuhn
              email                : matthias (at) opengis.ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Material 2.14
import QtQuick.Controls.Material.impl 2.14

import org.qgis 1.0
import org.qfield 1.0
import Theme 1.0
import QFieldControls 1.0

Rectangle {
  id: featureForm

  property FeatureListModelSelection selection
  property MapSettings mapSettings
  property DigitizingToolbar digitizingToolbar
  property ConfirmationToolbar moveFeaturesToolbar
  property BarcodeReader barcodeReader

  property color selectionColor
  property alias model: globalFeaturesList.model
  property alias extentController: featureListToolBar.extentController

  property bool allowEdit
  property bool allowDelete

  property bool multiSelection: false
  property bool fullScreenView: qfieldSettings.fullScreenIdentifyView
  property bool isVertical: parent.width < parent.height || parent.width < 300

  property bool canvasOperationRequested: digitizingToolbar.geometryRequested ||
                                          moveFeaturesToolbar.moveFeaturesRequested

  signal showMessage(string message)
  signal editGeometry

  function requestCancel() {
    featureFormList.requestCancel();
  }

  width: {
      if ( props.isVisible || featureForm.canvasOperationRequested )
      {
          if (fullScreenView || parent.width < parent.height || parent.width < 300)
          {
              parent.width
          }
          else
          {
              Math.min(Math.max( 200, parent.width / 2.6), parent.width)
          }
      }
      else
      {

          0
      }
  }
  height: {
     if ( props.isVisible || featureForm.canvasOperationRequested )
     {
         if (fullScreenView || parent.width > parent.height)
         {
             parent.height
         }
         else
         {
             isVertical = true
             Math.min(Math.max( 200, parent.height / 2 ), parent.height)
         }
     }
     else
     {
         0
     }
  }

  anchors.bottomMargin: featureForm.canvasOperationRequested ? featureForm.height : 0
  anchors.rightMargin: featureForm.canvasOperationRequested ? -featureForm.width : 0
  opacity: featureForm.canvasOperationRequested ? 0.5 : 1

  enabled: !featureForm.canvasOperationRequested
  visible: props.isVisible

  states: [
    State {
      name: "Hidden"
      StateChangeScript {
        script: {
          hide()
          if( featureFormList.state === "Edit" ){
            //e.g. tip on the canvas during an edit
            featureFormList.confirm()
          }
        }
      }
    },
    /* Shows a list of features */
    State {
      name: "FeatureList"
      PropertyChanges {
        target: globalFeaturesList
        shown: true

      }
      PropertyChanges {
        target: featureListToolBar
        state: "Indication"
      }
      StateChangeScript {
        script: {
          show()
          locatorItem.state = "off"
          if( featureFormList.state === "Edit" ){
            ///e.g. tip on the canvas during an edit
            featureFormList.confirm()
          }
        }
      }
    },
    /* Shows the form for the currently selected feature */
    State {
      name: "FeatureForm"
      PropertyChanges {
        target: globalFeaturesList
        shown: false
      }
      PropertyChanges {
        target: featureListToolBar
        state: "Navigation"
      }
      PropertyChanges {
        target: featureFormList
        state: "ReadOnly"

      }
    },
    /* Shows an editable form for the currently selected feature */
    State {
      name: "FeatureFormEdit"
      PropertyChanges {
        target: featureListToolBar
        state: "Edit"
      }
      PropertyChanges {
        target: featureFormList
        state: "Edit"
      }
    }

  ]
  state: "Hidden"

  clip: true

  QtObject {
    id: props

    property bool isVisible: false
  }

  ListView {
    id: globalFeaturesList

    anchors.top: featureListToolBar.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom

    property bool shown: false

    clip: true

    ScrollBar.vertical: ScrollBar {
      width: 6
      policy: globalFeaturesList.childrenRect.height > globalFeaturesList.height ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff

      contentItem: Rectangle {
        implicitWidth: 6
        implicitHeight: 25
        color: Theme.mainColor
      }
    }

    section.property: "layerName"
    section.labelPositioning: ViewSection.CurrentLabelAtStart | ViewSection.InlineLabels
    section.delegate: Component {
      /* section header: layer name */
      Rectangle {
        width: parent.width
        height: 30
        color: Theme.lightestGray

        Text {
          anchors { horizontalCenter: parent.horizontalCenter; verticalCenter: parent.verticalCenter }
          font.bold: true
          font.pointSize: Theme.resultFont.pointSize
          text: section
        }
      }
    }

    delegate: Rectangle {
      id: itemBackground
      anchors {
        left: parent ? parent.left : undefined
        right: parent ? parent.right: undefined
      }
      focus: true
      height: Math.max( 48, featureText.height )

      Ripple {
          clip: true
          width: parent.width
          height: parent.height
          pressed: mouseArea.pressed
          anchor: itemBackground
          active: mouseArea.pressed
          color: Material.rippleColor
      }

      CheckBox {
          anchors { leftMargin: 5; left: parent.left; verticalCenter: parent.verticalCenter }
          checked: featureSelected
          visible: featureForm.multiSelection
      }

      Text {
        id: featureText
        anchors {
          leftMargin: featureForm.multiSelection ? 50 : 10
          left: parent.left
          verticalCenter: parent.verticalCenter
        }
        font.bold: true
        font.pointSize: Theme.resultFont.pointSize
        text: display
      }

      Rectangle {
        anchors.left: parent.left
        height: parent.height
        width: 6
        color: featureForm.selectionColor
        opacity: index == featureForm.selection.focusedItem && featureForm.selection.model.selectedCount == 0 ? 1 : 0
        Behavior on opacity {
          PropertyAnimation {
            easing.type: Easing.OutQuart
          }
        }
      }

      MouseArea {
        id: mouseArea
        anchors.fill: parent

        onClicked: {
          if ( featureForm.multiSelection ) {
              featureForm.selection.toggleSelectedItem( index );
              if ( featureForm.selection.model.selectedCount == 0 ) {
                  featureFormList.model.featureModel.modelMode = FeatureModel.SingleFeatureModel
                  featureForm.multiSelection = false;
              }
              featureForm.selection.focusedItem = featureForm.selection.model.selectedCount > 0 ? index : -1;
          } else {
            featureFormList.model.featureModel.modelMode = FeatureModel.SingleFeatureModel
            featureForm.selection.focusedItem = index
            featureForm.state = "FeatureForm"
            featureForm.multiSelection = false;
          }

          featureFormList.model.applyFeatureModel()
        }

        onPressAndHold:
        {
          featureFormList.model.featureModel.modelMode = FeatureModel.MultiFeatureModel
          featureForm.selection.focusedItem = index
          featureForm.selection.toggleSelectedItem( index );
          featureForm.multiSelection = true;

        }
      }

      /* bottom border */
      Rectangle {
        anchors.bottom: parent.bottom
        height: 1
        color: "lightGray"
        width: parent.width
      }
    }

    /* bottom border */
    Rectangle {
      anchors.bottom: parent.bottom
      height: 1
      color: "lightGray"
      width: parent.width
    }

    onShownChanged: {
      if ( shown )
      {
        height = parent.height - featureListToolBar.height
      }
      else
      {
        height = 0
      }
    }

    Behavior on height {
      PropertyAnimation {
        easing.type: Easing.OutQuart
      }
    }
  }

  FeatureForm {
    id: featureFormList

    anchors.top: featureListToolBar.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    height: parent.height - globalFeaturesList.height

    digitizingToolbar: featureForm.digitizingToolbar
    barcodeReader: featureForm.barcodeReader

    model: AttributeFormModel {
      featureModel: FeatureModel {
        project: qgisProject
        currentLayer: featureForm.selection.focusedLayer
        feature: featureForm.selection.focusedFeature
        features: featureForm.selection.model.selectedFeatures
        cloudUserInformation: cloudConnection.userInformation
      }
    }

    focus: true

    visible: !globalFeaturesList.shown

    onCancelled: {
      featureForm.selection.focusedItemChanged()
      featureFormList.model.featureModel.reset()
      featureForm.state = featureForm.selection.model.selectedCount > 0 ? "FeatureList" : "FeatureForm"
      if (!qfieldSettings.autoSave) {
          displayToast( qsTr( "Changes discarded" ), 'warning' )
      }
    }
  }

  NavigationBar {
    id: featureListToolBar

    topMargin: featureForm.y == 0 ? mainWindow.sceneTopMargin : 0.0

    allowDelete: allowDelete
    model: globalFeaturesList.model
    selection: featureForm.selection
    multiSelection: featureForm.multiSelection
    extentController: FeaturelistExtentController {
      model: globalFeaturesList.model
      selection: featureForm.selection
      mapSettings: featureForm.mapSettings

      onFeatureFormStateRequested: {
        featureForm.state = "FeatureForm"
      }
    }

    onBackClicked: {
        featureForm.focus = true;
        if ( featureForm.state != "FeatureList" ) {
            featureForm.state = "FeatureList";
        } else {
            featureForm.state = "Hidden";
        }
    }

    onStatusIndicatorClicked: {
      featureForm.state = "FeatureList"
    }

    onStatusIndicatorSwiped: (direction) => {
      if (isVertical) {
        if (direction === 'up') {
          fullScreenView = true
        } else if (direction === 'down') {
          if ( fullScreenView ) {
            fullScreenView = false
          } else {
            featureForm.state = 'Hidden'
          }
        }
      } else {
        if (direction === 'left') {
          fullScreenView = true
        } else if (direction === 'right') {
          if ( fullScreenView ) {
            fullScreenView = false
          } else {
            featureForm.state = 'Hidden'
          }
        }
      }
    }

    onEditAttributesButtonClicked: {
        if( trackingModel.featureInTracking(selection.focusedLayer, selection.focusedFeature.id) )
        {
            displayToast( qsTr( "Stop tracking this feature to edit attributes" ) )
        }
        else
        {
            featureForm.state = "FeatureFormEdit"
        }
    }

    onEditGeometryButtonClicked: {
        if( trackingModel.featureInTracking(selection.focusedLayer, selection.focusedFeature.id) )
        {
            displayToast( qsTr( "Stop tracking this feature to edit geometry" ) )
        }
        else
        {
            editGeometry()
        }
    }

    onSave: {
        featureFormList.confirm()
        featureForm.state = featureForm.selection.model.selectedCount > 0 ? "FeatureList" : "FeatureForm"
        displayToast( qsTr( "Changes saved" ) )
    }

    onCancel: {
        featureForm.requestCancel();
    }

    onDestinationClicked: {
      navigation.setDestinationFeature(featureForm.selection.focusedFeature,featureForm.selection.focusedLayer)
      featureForm.state = "Hidden";
    }

    onMoveClicked: {
        if (featureForm.selection.focusedItem !== -1) {
            featureForm.state = "FeatureList"
            featureForm.multiSelection = true
            featureForm.selection.model.toggleSelectedItem(featureForm.selection.focusedItem)
            moveFeaturesToolbar.initializeMoveFeatures()
        }
    }

    onDuplicateClicked: {
        if (featureForm.selection.model.duplicateFeature(featureForm.selection.focusedLayer,featureForm.selection.focusedFeature)) {
          displayToast( qsTr( "Successfully duplicated feature" ) )

          featureForm.selection.focusedItem = -1
          featureForm.state = "FeatureList"
          featureForm.multiSelection = true
        }
    }

    onDeleteClicked: {
        var selectedFeatures = featureForm.selection.model.selectedFeatures
        var selectedFeature = selectedFeatures && selectedFeatures.length > 0 ? selectedFeatures[0] : null

        if(
            selectedFeature
            && featureForm.selection.focusedLayer
            && trackingModel.featureInTracking(featureForm.selection.focusedLayer, selectedFeature)
        )
        {
          displayToast( qsTr( "A number of features are being tracked, stop tracking to delete those" ) )
        }
        else
        {
          deleteDialog.show()
        }
    }

    onToggleMultiSelection: {
        featureForm.selection.focusedItem = -1;
        if ( featureForm.multiSelection ) {
            featureFormList.model.featureModel.modelMode = FeatureModel.SingleFeatureModel
            featureFormList.model.applyFeatureModel()
            featureForm.selection.model.clearSelection();
        } else {
            featureFormList.model.featureModel.modelMode = FeatureModel.MultiFeatureModel
        }
        featureForm.multiSelection = !featureForm.multiSelection;
        featureForm.focus = true;
    }

    onMultiEditClicked: {
        if (featureForm.selection.focusedItem == -1) {
          // focus on the first selected item to grab its layer
          featureForm.selection.focusedItem = 0;
        }
        featureForm.state = "FeatureFormEdit"
    }

    onMultiMergeClicked: {
        if( trackingModel.featureInTracking(featureForm.selection.focusedLayer, featureForm.selection.model.selectedFeatures) )
        {
          displayToast( qsTr( "A number of features are being tracked, stop tracking to merge those" ) )
        }
        else
        {
          mergeDialog.show()
        }
    }

    onMultiMoveClicked: {
        moveFeaturesToolbar.initializeMoveFeatures()
    }

    CoordinateTransformer {
        id: moveFeaturesTransformer
        sourceCrs: mapCanvas.mapSettings.destinationCrs
        destinationCrs: featureForm.selection.model.selectedLayer ? featureForm.selection.model.selectedLayer.crs : mapCanvas.mapSettings.destinationCrs
    }

    Connections {
        target: moveFeaturesToolbar

        function onMoveConfirmed() {
            moveFeaturesTransformer.sourcePosition = moveFeaturesToolbar.endPoint
            var translateX = moveFeaturesTransformer.projectedPosition.x
            var translateY = moveFeaturesTransformer.projectedPosition.y
            moveFeaturesTransformer.sourcePosition = moveFeaturesToolbar.startPoint
            translateX -= moveFeaturesTransformer.projectedPosition.x
            translateY -= moveFeaturesTransformer.projectedPosition.y
            featureForm.model.moveSelection(translateX, translateY)
        }
    }

    onMultiDuplicateClicked: {
        if  (featureForm.multiSelection) {
          if (featureForm.model.duplicateSelection()) {
              displayToast( qsTr( "Successfully duplicated selected features, list updated to show newly-created features" ) )
          }
        }
    }

    onMultiDeleteClicked: {
        if( trackingModel.featureInTracking(featureForm.selection.focusedLayer, featureForm.selection.model.selectedFeatures) )
        {
          displayToast( qsTr( "A number of features are being tracked, stop tracking to delete those" ) )
        }
        else
        {
          deleteDialog.show()
        }
    }
  }

  Keys.onReleased: (event) => {
      if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
          // if visible overlays (such as embedded feature forms) are present, don't take over
          if (ApplicationWindow.overlay.visibleChildren.length > 1 || (ApplicationWindow.overlay.visibleChildren.length === 1 && !toast.visible))
              return;

          if (state != "FeatureList") {
              if (featureListToolBar.state === "Edit") {
                  featureFormList.requestCancel();
              } else {
                  state = "FeatureList";
              }
          } else  {
              if (featureForm.multiSelection) {
                  featureForm.selection.model.clearSelection();
                  featureForm.selection.focusedItem = -1
                  featureForm.multiSelection = false;
              } else {
                  state = "Hidden";
              }
          }
          event.accepted = true;
      }
  }

  Behavior on width {
    PropertyAnimation {
      duration: parent.width > parent.height ? 250 : 0
      easing.type: Easing.OutQuart

      onRunningChanged: {
        if ( running )
          mapCanvasMap.freeze('formresize')
        else
          mapCanvasMap.unfreeze('formresize')
      }
    }
  }

  Behavior on height {
    PropertyAnimation {
      duration: parent.width < parent.height ? 250 : 0
      easing.type: Easing.OutQuart

      onRunningChanged: {
        if ( running )
          mapCanvasMap.freeze('formresize')
        else
          mapCanvasMap.unfreeze('formresize')
      }
    }
  }

  Behavior on anchors.rightMargin {
    PropertyAnimation {
      duration: 250
      easing.type: Easing.OutQuart

      onRunningChanged: {
        if ( running )
          mapCanvasMap.freeze('formresize')
        else
          mapCanvasMap.unfreeze('formresize')
      }
    }
  }

  Behavior on anchors.bottomMargin {
    PropertyAnimation {
      duration: 250
      easing.type: Easing.OutQuart

      onRunningChanged: {
        if ( running )
          mapCanvasMap.freeze('formresize')
        else
          mapCanvasMap.unfreeze('formresize')
      }
    }
  }

  Connections {
    target: globalFeaturesList.model

    function onRowsInserted(parent, first, VectorLayerStatic) {
      if ( model.rowCount() > 0 ) {
        state = "FeatureList"
      } else {
        showMessage( qsTr('No feature at this position') )
        state = "Hidden"
      }
    }

    function onCountChanged() {
      if ( model.rowCount() === 0 ) {
        state = "Hidden"
      }
    }

    function onModelReset() {
      if ( model.rowCount() > 0 ) {
        state = "FeatureList"
      } else {
        state = "Hidden"
      }
    }
  }

  function show()
  {
    props.isVisible = true
    focus = true
  }

  function hide()
  {
    props.isVisible = false;
    focus = false;

    fullScreenView = qfieldSettings.fullScreenIdentifyView;

    if ( !featureForm.canvasOperationRequested )
    {
      featureForm.multiSelection = false;
      featureFormList.model.featureModel.modelMode = FeatureModel.SingleFeatureModel;
      featureForm.selection.clear();
      if ( featureForm.selection.model ) {
        featureForm.selection.model.clearSelection();
      }
      model.clear();
    }
  }

  Dialog {
    id: mergeDialog
    parent: mainWindow.contentItem

    property int selectedCount: 0
    property string featureDisplayName: ''
    property bool isMerged: false

    visible: false
    modal: true
    font: Theme.defaultFont

    x: ( mainWindow.width - width ) / 2
    y: ( mainWindow.height - height ) / 2

    title: qsTr( "Merge feature(s)" )
    Label {
      width: parent.width
      wrapMode: Text.WordWrap
      text: qsTr( "Should the %n feature(s) selected really be merge?\n\nThe features geometries will be combined into feature '%1', which will keep its attributes.", "0", mergeDialog.selectedCount ).arg( mergeDialog.featureDisplayName )
    }

    standardButtons: Dialog.Ok | Dialog.Cancel
    onAccepted: {
      if ( isMerged ) {
        return;
      }

      isMerged = featureForm.model.mergeSelection()

      if ( isMerged ) {
        displayToast( qsTr( "Successfully merged %n feature(s)", "", selectedCount ) );
      } else {
        displayToast( qsTr( "Failed to merge %n feature(s)", "", selectedCount ), 'warning' );
      }

      visible = false
      featureForm.focus = true;
    }
    onRejected: {
      visible = false
      featureForm.focus = true;
    }

    function show() {
        this.isMerged = false;
        this.selectedCount = featureForm.model.selectedCount;
        this.featureDisplayName = FeatureUtils.displayName(featureForm.selection.focusedLayer,featureForm.model.selectedFeatures[0])
        this.open();
    }
  }

  Dialog {
    id: deleteDialog
    parent: mainWindow.contentItem

    property int selectedCount: 0
    property bool isDeleted: false

    visible: false
    modal: true
    font: Theme.defaultFont

    x: ( mainWindow.width - width ) / 2
    y: ( mainWindow.height - height ) / 2

    title: qsTr( "Delete feature(s)" )
    Label {
      width: parent.width
      wrapMode: Text.WordWrap
      text: qsTr( "Should the %n feature(s) selected really be deleted?", "0", deleteDialog.selectedCount )
    }

    standardButtons: Dialog.Ok | Dialog.Cancel
    onAccepted: {
      if ( isDeleted ) {
        return;
      }

      if  ( featureForm.multiSelection ) {
        isDeleted = featureForm.model.deleteSelection()
      } else {
        isDeleted = featureForm.selection.model.deleteFeature(featureForm.selection.focusedLayer,featureForm.selection.focusedFeature.id)
      }

      if ( isDeleted ) {
        displayToast( qsTr( "Successfully deleted %n feature(s)", "", selectedCount ) );
        if ( !featureForm.multiSelection ) {
          featureForm.selection.focusedItem = -1
          featureForm.state = "FeatureList"
        }
        if ( featureForm.selection.model.count === 0 )
          featureForm.state = "Hidden";
      } else {
        displayToast( qsTr( "Failed to delete %n feature(s)", "", selectedCount ), 'error' );
      }

      visible = false
      featureForm.focus = true;
    }
    onRejected: {
      visible = false
      featureForm.focus = true;
    }

    function show() {
        this.isDeleted = false;
        this.selectedCount = featureForm.multiSelection ? featureForm.model.selectedCount : 1;
        this.open();
    }
  }

  //if project changed we should hide drawer in case it's still open with old values
  //it pedals back, "simulates" a cancel without touching anything, but does not reset the model
  Connections {
    target: qgisProject

    function onLayersWillBeRemoved(layerIds) {
        if( state != "FeatureList" ) {
          if( featureListToolBar.state === "Edit"){
              featureForm.state = "FeatureForm"
              displayToast( qsTr( "Changes discarded" ), 'warning' )
          }
          state = "FeatureList"
        }
        state = "Hidden"
    }
  }
}
