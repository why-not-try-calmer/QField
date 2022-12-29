/***************************************************************************
  submodel.h - SubModel

 ---------------------
 begin                : 16.9.2016
 copyright            : (C) 2016 by Matthias Kuhn
 email                : matthias@opengis.ch
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef SUBMODEL_H
#define SUBMODEL_H

#include <QAbstractItemModel>
#include <QPointer>
#include <QStandardItem>

class SubModel : public QAbstractItemModel
{
    Q_OBJECT

    Q_PROPERTY( bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged )
    Q_PROPERTY( QAbstractItemModel *model READ model WRITE setModel NOTIFY modelChanged )
    Q_PROPERTY( QModelIndex rootIndex READ rootIndex WRITE setRootIndex NOTIFY rootIndexChanged )

  public:
    explicit SubModel( QObject *parent = nullptr );
    QModelIndex index( int row, int column, const QModelIndex &parent ) const override;
    QModelIndex parent( const QModelIndex &child ) const override;
    int rowCount( const QModelIndex &parent ) const override;
    int columnCount( const QModelIndex &parent ) const override;
    QVariant data( const QModelIndex &index, int role ) const override;
    bool setData( const QModelIndex &index, const QVariant &value, int role = Qt::EditRole ) override;
    QHash<int, QByteArray> roleNames() const override;

    QModelIndex rootIndex() const;
    void setRootIndex( const QModelIndex &rootIndex );

    QAbstractItemModel *model() const;
    void setModel( QAbstractItemModel *model );

    bool enabled() const { return mEnabled; }
    void setEnabled( bool enabled );

    bool isInSubModel( const QModelIndex &sourceIndex ) const;

  signals:
    void modelChanged();
    void enabledChanged();
    void rootIndexChanged();

  private slots:
    void onRowsInserted( const QModelIndex &parent, int first, int last );
    void onRowsAboutToBeRemoved( const QModelIndex &parent, int first, int last );
    void onModelReset();
    void onDataChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>() );

  private:
    QModelIndex mapFromSource( const QModelIndex &sourceIndex ) const;
    QModelIndex mapToSource( const QModelIndex &index ) const;
    void handleModelConnection( bool disconnecting = false ) const;

    bool mEnabled = true;
    QPointer<QAbstractItemModel> mModel;
    QPersistentModelIndex mRootIndex;

    // Map internal id to parent index
    mutable QHash<qintptr, QModelIndex> mMappings;
};

#endif // SUBMODEL_H
