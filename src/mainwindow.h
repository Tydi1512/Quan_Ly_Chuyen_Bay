#pragma once

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Planes.h"
#include "Passengers.h"
#include "Flights.h"

enum PageType
{
    PLANE,
    FLIGHT,
    CUSTOMER,
    TICKET,
    REPORT,
    NONE_PAGE
};

enum PopupType
{
    ADD_POPUP,
    UPDATE_POPUP,
    DELETE_POPUP,
    FIND_POPUP,
    NON_POPUP
};

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void OnPlanesButtonClicked();
    void OnFlightsButtonClicked();
    void OnCustomerButtonClicked();
    void OnTicketButtonClicked();
    void OnReportButtonClicked();
    void onsearchSeatButtonclicked();
    void OnSearchPlaneButtonClicked(); // Hàm xử lý tìm số hiệu máy bay
    void OnAddButtonClicked();
    void OnUpdateButtonClicked();
    void OnRemoveButtonClicked();
    void OnOkButtonClicked();
    void OnCancelButtonClicked();
    void OnTableWidgetCellClicked(int row, int column);
    void OnTypePassengerViewChanged();
    void OnFindButtonClicked();

private:
    Ui::MainWindow *ui;
    PageType currentPage;
    PopupType currentPopup;

    const int rowsPerPage = 10;
    int selectedRow;
    int locationChoose;

    PlanesList planesList;
    FlightManager flightManager;
    PassengerManager passengerManager;
    QStringList idPassengerList;

private:
    void DisplayCurrentPage();
    void DisplayPassengerPage();
    void DisPlayPopup(PopupType type);

    void SetStatusButton(bool status);
    void SetStatusPopupWidget(bool status, PopupType type);

    bool PlaneValidateInput();
    bool FlightValidateInput();
    bool PassengerValidateInput();



    void GetIDPassengerList(AVLTree passenger);
    void focusOutEvent();
    void HideWarring();

    void CreateButtonGrid(int c, int r, map<string, int> passengerList);
    void HandleButtonGridClicked(int index);

    void ShowAlert(QString content);
    void SaveFile();

    void UpdateFlightTimesOfPlane();
    void ClearDataInputBox();
};
#endif // MAINWINDOW_H
