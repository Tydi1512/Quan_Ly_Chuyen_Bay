#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "buttongridwidget.h"
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentPage(PageType::NONE_PAGE)
    , currentPopup(PopupType::NON_POPUP)
    , selectedRow(-1)
    , locationChoose(-1)
{

    // Read data from file
    planesList.readFilePlane("/Users/huynhdat/Documents/QuanLyVeMayBay/PlanesData.txt");
    flightManager.readFile("/Users/huynhdat/Documents/QuanLyVeMayBay/FlightsData.txt");
    passengerManager.readFilePassenger("/Users/huynhdat/Documents/QuanLyVeMayBay/PassengersData.txt");


    // Setup plane table
    ui->setupUi(this);
    connect(ui->planesButton, &QPushButton::clicked, this, &MainWindow::OnPlanesButtonClicked);
    connect(ui->flightsButton, &QPushButton::clicked, this, &MainWindow::OnFlightsButtonClicked);
    connect(ui->customerButton, &QPushButton::clicked, this, &MainWindow::OnCustomerButtonClicked);
    connect(ui->ticketButton, &QPushButton::clicked, this, &MainWindow::OnTicketButtonClicked);
    connect(ui->reportButton, &QPushButton::clicked, this, &MainWindow::OnReportButtonClicked);

    // Setup button
    SetStatusButton(false);
    ui->updateButton->setVisible(false);
    ui->removeButton->setVisible(false);
    ui->searchSeatButton->setVisible(false);
    ui->seatInput->setVisible(false);
    ui->searchInput->setVisible(false);
    ui->searchButton->setVisible(false);

    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::OnAddButtonClicked);
    connect(ui->updateButton, &QPushButton::clicked, this, &MainWindow::OnUpdateButtonClicked);
    connect(ui->removeButton, &QPushButton::clicked, this, &MainWindow::OnRemoveButtonClicked);
    connect(ui->tableInfo, &QTableWidget::cellClicked, this, &MainWindow::OnTableWidgetCellClicked);
    connect(ui->comboBox_1, &QComboBox::currentIndexChanged, this, &MainWindow::OnTypePassengerViewChanged);
    connect(ui->searchButton, &QPushButton::clicked, this, &MainWindow::OnSearchPlaneButtonClicked);

    connect(ui->searchSeatButton, &QPushButton::clicked, this, &MainWindow::onsearchSeatButtonclicked);


    // Setup popup
    SetStatusPopupWidget(false, PopupType::NON_POPUP);
    connect(ui->okButton, &QPushButton::clicked, this, &MainWindow::OnOkButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &MainWindow::OnCancelButtonClicked);
    HideWarring();

    ui->flightInfo->setVisible(false);

    ui->gridLayoutWidget->setStyleSheet("background-color: #caf7f4;");
    ui->gridLayoutWidget->lower();
    ui->popupBG->setStyleSheet("background-color: #4A90E2;");


    focusOutEvent();
    ui->alertLabel->setVisible(false);

}

MainWindow::~MainWindow()
{
    SaveFile();
    delete ui;
}

void MainWindow::HideWarring()
{
    ui->warring_0->setVisible(false);
    ui->warring_1->setVisible(false);
    ui->warring_2->setVisible(false);
    ui->warring_3->setVisible(false);
    ui->warring_4->setVisible(false);
    ui->warring_5->setVisible(false);

    ui->warring_0->setStyleSheet("color: red;");
    ui->warring_1->setStyleSheet("color: red;");
    ui->warring_2->setStyleSheet("color: red;");
    ui->warring_3->setStyleSheet("color: red;");
    ui->warring_4->setStyleSheet("color: red;");
    ui->warring_5->setStyleSheet("color: red;");

    ui->input1->setStyleSheet("border: 1px solid black;");
    ui->input2->setStyleSheet("border: 1px solid black;");
    ui->input3->setStyleSheet("border: 1px solid black;");
    ui->input4->setStyleSheet("border: 1px solid black;");
    ui->idPlane->setStyleSheet("border: 1px solid black;");
    ui->statusFlight->setStyleSheet("border: 1px solid black;");
}

bool MainWindow::PlaneValidateInput() {
    bool valib = true;
    string idP = ui->input1->text().toStdString();
    string type = ui->input2->text().toStdString();
    int row = ui->input3->text().toInt();
    int column = ui->input4->text().toInt();

    // Xử lý idP: Loại bỏ khoảng trắng dư thừa, chuyển thành chữ in hoa
    idP.erase(remove_if(idP.begin(), idP.end(), ::isspace), idP.end()); // Loại bỏ khoảng trắng
    std::transform(idP.begin(), idP.end(), idP.begin(), ::toupper);    // Chuyển thành in hoa
    ui->input1->setText(QString::fromStdString(idP));

    // Kiểm tra idP
    if (idP.empty() || (planesList.findPlane(idP) > -1 && currentPopup == PopupType::ADD_POPUP)) {
        ui->input1->setStyleSheet("border: 1px solid red;");
        ui->warring_0->setText("Số hiệu trống hoặc đã có");
        ui->warring_0->setVisible(true);
        valib = false;
    } else if (idP.length() > 15) { // Kiểm tra độ dài tối đa
        ui->input1->setStyleSheet("border: 1px solid red;");
        ui->warring_0->setText("Số hiệu tối đa 15 ký tự");
        ui->warring_0->setVisible(true);
        valib = false;
    } else if (idP.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") != string::npos) { // Kiểm tra ký tự không hợp lệ
        ui->input1->setStyleSheet("border: 1px solid red;");
        ui->warring_0->setText("Số hiệu chỉ chứa chữ và số (không chứa ký tự đặc biệt hoặc khoảng trắng)");
        ui->warring_0->setVisible(true);
        valib = false;
    } else {
        ui->input1->setStyleSheet("border: 1px solid black;");
        ui->warring_0->setVisible(false);
    }

    // Xử lý type: Loại bỏ khoảng trắng dư thừa, chuyển thành chữ in hoa
    type.erase(remove_if(type.begin(), type.end(), ::isspace), type.end()); // Loại bỏ khoảng trắng
    std::transform(type.begin(), type.end(), type.begin(), ::toupper);      // Chuyển thành in hoa
    ui->input2->setText(QString::fromStdString(type));

    // Kiểm tra type
    if (type.empty()) {
        ui->input2->setStyleSheet("border: 1px solid red;");
        ui->warring_1->setText("Loại máy bay trống");
        ui->warring_1->setVisible(true);
        valib = false;
    } else if (type.length() > 40) { // Kiểm tra độ dài tối đa
        ui->input2->setStyleSheet("border: 1px solid red;");
        ui->warring_1->setText("Loại máy bay tối đa 40 ký tự");
        ui->warring_1->setVisible(true);
        valib = false;
    } else if (type.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") != string::npos) { // Kiểm tra ký tự không hợp lệ
        ui->input2->setStyleSheet("border: 1px solid red;");
        ui->warring_1->setText("Loại máy bay chỉ chứa chữ và số (không chứa ký tự đặc biệt hoặc khoảng trắng)");
        ui->warring_1->setVisible(true);
        valib = false;
    } else {
        ui->warring_1->setVisible(false);
        ui->input2->setStyleSheet("border: 1px solid black;");
    }

    // Kiểm tra hàng
    if (row < 1 || row > 20) {
        ui->warring_3->setText("Nhập số từ 1-20");
        ui->warring_3->setVisible(true);
        valib = false;
    } else {
        ui->warring_3->setVisible(false);
    }

    // Kiểm tra cột
    if (column < 1 || column > 100) {
        ui->warring_4->setText("Nhập số từ 1-100");
        ui->warring_4->setVisible(true);
        valib = false;
    } else {
        ui->warring_4->setVisible(false);
    }

    return valib;
}

bool MainWindow::FlightValidateInput()
{
    bool valib = true;

    // Lấy dữ liệu từ giao diện
    string idF = ui->input1->text().toStdString();  // Mã chuyến bay
    string idP = ui->idPlane->currentText().toStdString();  // Mã máy bay
    string location = ui->input3->text().toStdString();  // Điểm đến

    // === XỬ LÝ idF (Mã chuyến bay) ===
    // Loại bỏ khoảng trắng dư thừa và chuyển thành chữ in hoa
    idF.erase(remove_if(idF.begin(), idF.end(), ::isspace), idF.end());
    std::transform(idF.begin(), idF.end(), idF.begin(), ::toupper);

    // Cập nhật lại giao diện với chuỗi đã chuẩn hóa
    ui->input1->setText(QString::fromStdString(idF));

    // Kiểm tra idF (Trống hoặc trùng lặp)
    if (idF.empty() || (flightManager.findFlightByIdFlight(idF.c_str()) != NULL && currentPopup == PopupType::ADD_POPUP))
    {
        ui->input1->setStyleSheet("border: 1px solid red;");
        ui->warring_0->setText("Mã chuyến bay trống hoặc đã tồn tại");
        ui->warring_0->setVisible(true);
        valib = false;
    }
    else if (idF.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") != string::npos) // Kiểm tra ký tự không hợp lệ
    {
        ui->input1->setStyleSheet("border: 1px solid red;");
        ui->warring_0->setText("Mã chuyến bay chỉ chứa chữ và số (không có ký tự đặc biệt hoặc khoảng trắng)");
        ui->warring_0->setVisible(true);
        valib = false;
    }
    else
    {
        ui->input1->setStyleSheet("border: 1px solid black;");
        ui->warring_0->setVisible(false);
    }

    // === XỬ LÝ idP (Mã máy bay) ===
    // Loại bỏ khoảng trắng dư thừa và chuyển thành chữ in hoa
    idP.erase(remove_if(idP.begin(), idP.end(), ::isspace), idP.end());
    std::transform(idP.begin(), idP.end(), idP.begin(), ::toupper);

    // Cập nhật lại giao diện với chuỗi đã chuẩn hóa
    ui->idPlane->setCurrentText(QString::fromStdString(idP));

    // Kiểm tra idP (Trống)
    if (idP.empty())
    {
        ui->idPlane->setStyleSheet("border: 1px solid red;");
        valib = false;
    }
    else
    {
        ui->idPlane->setStyleSheet("border: 1px solid black;");
    }

    // === XỬ LÝ location (Điểm đến) ===
    // Loại bỏ khoảng trắng dư thừa và chuẩn hóa
    location.erase(remove_if(location.begin(), location.end(), ::isspace), location.end());
    std::transform(location.begin(), location.end(), location.begin(), ::toupper);

    // Cập nhật lại giao diện với chuỗi đã chuẩn hóa
    ui->input3->setText(QString::fromStdString(location));

    // Kiểm tra location (Trống)
    if (location.empty())
    {
        ui->input3->setStyleSheet("border: 1px solid red;");
        ui->warring_3->setText("Điểm đến trống");
        ui->warring_3->setVisible(true);
        valib = false;
    }
    else if (location.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") != string::npos) // Kiểm tra ký tự không hợp lệ
    {
        ui->input3->setStyleSheet("border: 1px solid red;");
        ui->warring_3->setText("Điểm đến chỉ chứa chữ và số (không có ký tự đặc biệt hoặc khoảng trắng)");
        ui->warring_3->setVisible(true);
        valib = false;
    }
    else
    {
        ui->input3->setStyleSheet("border: 1px solid black;");
        ui->warring_3->setVisible(false);
    }

    return valib;
}


bool MainWindow::PassengerValidateInput() {
    bool valid = true;

    // Xử lý CCCD: Kiểm tra 12 số và không chứa ký tự đặc biệt hoặc khoảng trắng
    QString cccd = ui->input1->text().remove(QRegularExpression("\\s")); // Xóa khoảng trắng
    if (cccd.length() != 12 || !cccd.contains(QRegularExpression("^[0-9]{12}$"))) {
        valid = false;
        ui->input1->setStyleSheet("border: 1px solid red;");
        ui->warring_0->setText("CCCD phải là 12 số");
        ui->warring_0->setVisible(true);
    } else {
        ui->warring_0->setVisible(false);
        ui->input1->setStyleSheet("border: 1px solid black;");
    }

    // Xử lý Họ: Chuyển thành chữ hoa và kiểm tra độ dài
    QString firstName = ui->input3->text().toUpper().remove(QRegularExpression("[^A-Z]")); // Chỉ giữ chữ cái và chuyển sang viết hoa
    ui->input3->setText(firstName); // Cập nhật lại trường Họ với chữ hoa
    if (firstName.length() < 2 || firstName.length() > 15) {
        valid = false;
        ui->input3->setStyleSheet("border: 1px solid red;");
        ui->warring_3->setText("Họ phải từ 2 đến 15 chữ cái, không chứa khoảng trắng hoặc ký tự đặc biệt");
        ui->warring_3->setVisible(true);
    } else {
        ui->warring_3->setVisible(false);
        ui->input3->setStyleSheet("border: 1px solid black;");
    }

    // Xử lý Tên: Chuyển thành chữ hoa và kiểm tra độ dài
    QString lastName = ui->input4->text().toUpper().remove(QRegularExpression("[^A-Z]")); // Chỉ giữ chữ cái và chuyển sang viết hoa
    ui->input4->setText(lastName); // Cập nhật lại trường Tên với chữ hoa
    if (lastName.length() < 2 || lastName.length() > 15) {
        valid = false;
        ui->input4->setStyleSheet("border: 1px solid red;");
        ui->warring_4->setText("Tên phải từ 2 đến 15 chữ cái, không chứa khoảng trắng hoặc ký tự đặc biệt");
        ui->warring_4->setVisible(true);
    } else {
        ui->warring_4->setVisible(false);
        ui->input4->setStyleSheet("border: 1px solid black;");
    }

    // Kiểm tra Giới tính
    if (ui->statusFlight->currentIndex() == 0) { // Giá trị mặc định là "Chọn giới tính"
        valid = false;
        ui->statusFlight->setStyleSheet("border: 1px solid red;");
        ui->warring_gender->setText("<font color='red'>Vui lòng chọn giới tính</font>");
        ui->warring_gender->setVisible(true);
    } else {
        ui->warring_gender->setVisible(false);
        ui->statusFlight->setStyleSheet("border: 1px solid black;");
    }

    return valid;
}





void MainWindow::OnPlanesButtonClicked()
{
    currentPage = PLANE;
    SetStatusPopupWidget(false, PopupType::NON_POPUP);
    int size = planesList.size;
    int rowHeight = ui->tableInfo->verticalHeader()->defaultSectionSize();
    int maxRowsToShow = 10;

    focusOutEvent();
    SetStatusButton(true);
    ui->searchInput->setVisible(true);
    ui->searchButton->setVisible(true);
    // Ẩn các nút liên quan đến tab khác
    ui->searchSeatButton->setVisible(false);
    ui->seatInput->setVisible(false);
    ui->cancelButton->setVisible(false);

    // Xóa và cài đặt lại nội dung bảng
    ui->tableInfo->clearContents();
    ui->tableInfo->setColumnCount(3);
    ui->tableInfo->setHorizontalHeaderLabels({"Số Hiệu", "Loại", "Số Chỗ"});
    ui->tableInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableInfo->setMaximumHeight(rowHeight * maxRowsToShow + ui->tableInfo->horizontalHeader()->height());
    ui->tableInfo->setRowCount(size);

    int columnWidth = ui->tableInfo->width() / 3;
    for (int i = 0; i < 3; ++i) {
        ui->tableInfo->setColumnWidth(i, columnWidth);
    }

    for (int i = 0; i < size; ++i)
    {
        Plane* plane = planesList.data[i];
        ui->tableInfo->setItem(i, 0, new QTableWidgetItem(plane->idPlane));
        ui->tableInfo->setItem(i, 1, new QTableWidgetItem(plane->type));
        ui->tableInfo->setItem(i, 2, new QTableWidgetItem(QString::number(plane->seats)));
    }

    // Đổi text nút về "Sửa"
    ui->updateButton->setText("Sửa");
}


void MainWindow::OnFlightsButtonClicked()
{
    currentPage = FLIGHT;
    SetStatusPopupWidget(false, PopupType::NON_POPUP);
    int rowHeight = ui->tableInfo->verticalHeader()->defaultSectionSize();
    int maxRowsToShow = 10;
    int size = flightManager.getSize();
    int columnNumber = 6;
    focusOutEvent();
    SetStatusButton(true);

    ui->searchSeatButton->setVisible(false); // Ẩn nút tìm ghế
    ui->seatInput->setVisible(false);        // Ẩn ô nhập mã ghế
    ui->searchInput->setVisible(false);
    ui->searchButton->setVisible(false);


    // Bộ lọc
    // ui->comboBox_1->clear();
    // ui->comboBox_1->setVisible(true);
    // ui->comboBox_2->clear();
    // ui->comboBox_2->setVisible(true);
    // ui->comboBox_3->clear();
    // ui->comboBox_3->setVisible(true);
    // UI

    ui->tableInfo->clearContents();
    ui->tableInfo->setColumnCount(columnNumber);
    ui->tableInfo->setHorizontalHeaderLabels({"Mã chuyến bay", "Số hiệu máy bay", "Thời gian", "Điểm đến", "Trạng thái", "Số người"});
    ui->tableInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableInfo->setMaximumHeight(rowHeight * maxRowsToShow + ui->tableInfo->horizontalHeader()->height());
    ui->tableInfo->setRowCount(size);
    int columnWidth = ui->tableInfo->width() / columnNumber;
    for (int i = 0; i < columnNumber; ++i) {
        ui->tableInfo->setColumnWidth(i, columnWidth);
    }

    auto flight = flightManager.flightList;
    for (int i = 0; i < size; ++i)
    {
        ui->tableInfo->setItem(i, 0, new QTableWidgetItem(flight->info.idFlight));
        ui->tableInfo->setItem(i, 1, new QTableWidgetItem(flight->info.idPlane));
        ui->tableInfo->setItem(i, 2, new QTableWidgetItem(flight->info.date.formatStr().c_str()));
        ui->tableInfo->setItem(i, 3, new QTableWidgetItem(flight->info.arrive));
        string s;
        switch (flight->info.status) {
        case CANCLE_FLIGHT:
            s = "Cancle";
            break;
        case HAVE_TICKET:
            s = "Have ticket";
            break;
        case OUT_OF_TICKET:
            s = "Out of ticket";
            break;
        case COMPLETE_FLIGHT:
            s = "Complete";
            break;
        default:
            s = "";
            break;
        }
        if (flight->info.totalTicket == planesList.findPlaneByID(flight->info.idPlane)->seats)
        {
            s = "Out of ticket";
        }
        ui->tableInfo->setItem(i, 4, new QTableWidgetItem(s.c_str()));
        string textSeats = to_string(flight->info.totalTicket) + "/" + to_string(planesList.findPlaneByID(flight->info.idPlane)->seats);
        ui->tableInfo->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(textSeats)));
        flight = flight->next;
        // Đổi text nút về "Sửa"
        ui->updateButton->setText("Sửa");
    }
}

void MainWindow::OnCustomerButtonClicked()
{
    currentPage = CUSTOMER;
    focusOutEvent();
    SetStatusPopupWidget(false, PopupType::NON_POPUP);
    ui->comboBox_1->setCurrentIndex(0);

    int rowHeight = ui->tableInfo->verticalHeader()->defaultSectionSize();
    int maxRowsToShow = 10;
    int columnNumber = 5;
    focusOutEvent();
    //SetStatusButton(true);

    ui->searchSeatButton->setVisible(false); // Ẩn nút tìm ghế
    ui->seatInput->setVisible(false);        // Ẩn ô nhập mã ghế
    ui->searchInput->setVisible(false);
    ui->searchButton->setVisible(false);


    ui->tableInfo->setVisible(true);
    ui->comboBox_1->setVisible(true);
    ui->comboBox_1->clear();
    //ui->findPassenger->addItem("Tất cả");
    auto flight = flightManager.flightList;
    while (flight != NULL)
    {
        ui->comboBox_1->addItem(flight->info.idFlight);
        flight = flight->next;
    }


    ui->tableInfo->clearContents();
    ui->tableInfo->setColumnCount(columnNumber);
    ui->tableInfo->setHorizontalHeaderLabels({"CCCD", "Họ", "Tên", "Giới tính", "Vé số"});
    ui->tableInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableInfo->setMaximumHeight(rowHeight * maxRowsToShow + ui->tableInfo->horizontalHeader()->height());
    //ui->tableInfo->setRowCount(size);
    int columnWidth = ui->tableInfo->width() / columnNumber;
    for (int i = 0; i < columnNumber; ++i)
    {
        ui->tableInfo->setColumnWidth(i, columnWidth);
    }

    DisplayPassengerPage();
}

void MainWindow::OnTicketButtonClicked()
{
    currentPage = TICKET;
    focusOutEvent();
    SetStatusPopupWidget(false, PopupType::NON_POPUP);

    ui->tableInfo->clearContents();
    ui->tableInfo->setVisible(false);

    ui->searchSeatButton->setVisible(true); // Hiện nút tìm ghế
    ui->seatInput->setVisible(true);        // Hiện ô nhập mã ghế
    ui->searchInput->setVisible(false);
    ui->searchButton->setVisible(false);

    // ui->addButton->setVisible(true);
    // ui->addButton->setText("Mua vé");
    ui->flightInfo->setVisible(true);
    ui->flightInfo->setText("VUI LÒNG CHỌN GHẾ NGỒI\nĐỏ: đã có người mua\t\tXám: có thể mua");

    ui->updateButton->setVisible(true);
    ui->updateButton->setText("Hủy chuyến");

    ui->comboBox_1->setVisible(true);
    ui->comboBox_1->clear();
    //ui->findPassenger->addItem("Tất cả");
    auto flight = flightManager.flightList;
    while (flight != NULL)
    {
        ui->comboBox_1->addItem(flight->info.idFlight);
        flight = flight->next;
    }
}

void MainWindow::OnReportButtonClicked()
{
    currentPage = REPORT;
    focusOutEvent();
    SetStatusPopupWidget(false, PopupType::NON_POPUP);

    int columnNumber = 2;
    int rowHeight = ui->tableInfo->verticalHeader()->defaultSectionSize();
    int maxRowsToShow = 10;

    ui->tableInfo->clearContents();
    ui->tableInfo->setVisible(true);

    ui->searchSeatButton->setVisible(false);
    ui->seatInput->setVisible(false);
    ui->searchInput->setVisible(false);
    ui->searchButton->setVisible(false);


    int size = planesList.size;

    ui->tableInfo->setColumnCount(columnNumber);
    ui->tableInfo->setHorizontalHeaderLabels({"Số hiệu máy bay", "Số lần bay"});
    ui->tableInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableInfo->setMaximumHeight(rowHeight * maxRowsToShow + ui->tableInfo->horizontalHeader()->height());
    ui->tableInfo->setRowCount(size);
    int columnWidth = ui->tableInfo->width() / columnNumber;
    for (int i = 0; i < columnNumber; ++i)
    {
        ui->tableInfo->setColumnWidth(i, columnWidth);
    }

    // Update fly time
    UpdateFlightTimesOfPlane();

    int* sortedPlane = new int[size];
    for (int i = 0; i < size; i++)
        sortedPlane[i] = i;
    planesList.bubbleSortPlane(sortedPlane);
    for (int i = 0; i < size; ++i)
    {
        Plane* plane = planesList.data[sortedPlane[i]];
        ui->tableInfo->setItem(i, 0, new QTableWidgetItem(plane->idPlane));
        ui->tableInfo->setItem(i, 1, new QTableWidgetItem(QString::number(plane->flyTimes)));
    }
}

void MainWindow::OnTypePassengerViewChanged()
{
    if (currentPage == CUSTOMER)
    {
        ui->tableInfo->clear();
        ui->tableInfo->setHorizontalHeaderLabels({"CCCD", "Họ", "Tên", "Giới tính", "Vé số"});
        auto currentFlight = flightManager.findFlightByIdFlight(ui->comboBox_1->currentText().toStdString().c_str());
        if (!currentFlight)
        {
            return;
        }

        string dataF = currentFlight->info.date.formatStr();
        string text =   "DANH SÁCH HÀNH KHÁCH THUỘC CHUYẾN BAY: " + string(currentFlight->info.idFlight) +
                      "\nNgày giờ khởi hành: " + dataF +
                      "\tNơi đến: " + currentFlight->info.arrive;
        ui->flightInfo->setText(QString::fromStdString(text));
        ui->flightInfo->setVisible(true);

        int size = currentFlight->info.passengers.size();
        ui->tableInfo->setRowCount(size);

        DisplayPassengerPage();
    }
    else if (currentPage == TICKET)
    {
        auto currentFlight = flightManager.findFlightByIdFlight(ui->comboBox_1->currentText().toStdString().c_str());
        if (currentFlight)
        {
            auto plane = planesList.findPlaneByID(currentFlight->info.idPlane);
            ui->gridLayoutWidget->setVisible(true);
            CreateButtonGrid(plane->column, plane->row, currentFlight->info.passengers);
        }
    }
}

void MainWindow::DisplayCurrentPage()
{

}

void MainWindow::DisplayPassengerPage() {
    // Lấy ID chuyến bay hiện tại từ comboBox
    string currentFlight = ui->comboBox_1->currentText().toStdString();
    if (currentFlight.empty()) {
        return;
    }

    // Kiểm tra nếu chuyến bay tồn tại
    auto flightPtr = flightManager.findFlightByIdFlight(currentFlight.c_str());
    if (!flightPtr) {
        qDebug() << "Không tìm thấy chuyến bay với ID:" << QString::fromStdString(currentFlight);
        return;
    }

    // Lấy thông tin chuyến bay
    auto& flight = flightPtr->info;
    string flightInfoText = "DANH SÁCH HÀNH KHÁCH THUỘC CHUYẾN BAY: " + string(flight.idFlight) +
                            "\nNgày giờ khởi hành: " + flight.date.formatStr() +
                            "\tNơi đến: " + flight.arrive;
    ui->flightInfo->setText(QString::fromStdString(flightInfoText));
    ui->flightInfo->setVisible(true);

    // Xóa các hàng cũ trong table trước khi hiển thị dữ liệu mới
    ui->tableInfo->clearContents();
    ui->tableInfo->setRowCount(0);

    int row = 0;
    for (const auto& it : flight.passengers) {
        // Kiểm tra nếu hành khách tồn tại
        auto passenger = passengerManager.findPassenger(passengerManager.passengerList, it.first.c_str());
        if (!passenger) {
            qDebug() << "Không tìm thấy hành khách với ID:" << QString::fromStdString(it.first);
            continue;
        }

        // Thêm một hàng mới cho mỗi hành khách
        ui->tableInfo->insertRow(row);

        // Điền thông tin hành khách vào các cột
        ui->tableInfo->setItem(row, 0, new QTableWidgetItem(passenger->data.idPass));
        ui->tableInfo->setItem(row, 1, new QTableWidgetItem(passenger->data.firstName));
        ui->tableInfo->setItem(row, 2, new QTableWidgetItem(passenger->data.lastName));

        // Hiển thị giới tính
        QString genderText = (passenger->data.gender == 1) ? "Nam" : "Nữ";
        ui->tableInfo->setItem(row, 3, new QTableWidgetItem(genderText));

        // Tính toán vị trí ghế ngồi
        int index = flight.findLocationPassengerByID(passenger->data.idPass);
        auto planePtr = planesList.findPlaneByID(flight.idPlane);
        if (!planePtr) {
            qDebug() << "Không tìm thấy máy bay với ID:" << QString::fromStdString(flight.idPlane);
            continue;
        }
        int rowNumbers = planePtr->row;
        int seatRow = index / rowNumbers;
        int seatCol = index % rowNumbers + 1;
        string seatLocation = static_cast<char>(seatRow + 'A') + std::to_string(seatCol);

        ui->tableInfo->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(seatLocation)));
        row++;
    }
}



void MainWindow::GetIDPassengerList(AVLTree passenger)
{
    if (passenger!=NULL)
    {
        idPassengerList.push_back(passenger->data.idPass);
        //su dung de quy de duyet tiep cay con trai
        GetIDPassengerList(passenger->pleft);
        //su dung de quy de duyet tiep cay con phai
        GetIDPassengerList(passenger->pright);
    }
}

void MainWindow::OnAddButtonClicked()
{
    currentPopup = PopupType::ADD_POPUP;
    SetStatusPopupWidget(false, PopupType::NON_POPUP);
    DisPlayPopup(PopupType::ADD_POPUP);
};

void MainWindow::OnUpdateButtonClicked()
{
    currentPopup = PopupType::UPDATE_POPUP;
    SetStatusPopupWidget(false, PopupType::NON_POPUP);
    DisPlayPopup(PopupType::UPDATE_POPUP);
};

void MainWindow::OnRemoveButtonClicked()
{
    currentPopup = PopupType::DELETE_POPUP;
    SetStatusPopupWidget(false, PopupType::NON_POPUP);
    DisPlayPopup(PopupType::DELETE_POPUP);
    if (selectedRow >= 0)
    {
        // Xóa dòng được chọn
        QString data = ui->tableInfo->item(selectedRow, 0)->text();
        switch (currentPage) {
        case PageType::PLANE:
            planesList.removePlane(selectedRow);
            break;
        case PageType::FLIGHT:
            flightManager.deleteFlightByID(data.toStdString());
            break;
        case PageType::CUSTOMER:
            passengerManager.deletePassengerByID(data.toStdString());
            break;
        default:
            break;
        }

        ui->tableInfo->removeRow(selectedRow);
        selectedRow = -1;
        ui->removeButton->setVisible(false);
    }
};

void MainWindow::OnCancelButtonClicked()
{
    SetStatusPopupWidget(false, PopupType::NON_POPUP);

};



void MainWindow::OnOkButtonClicked()
{
    switch (currentPage) {
    case PageType::PLANE:
        if (PlaneValidateInput())
        {
            if (currentPopup == PopupType::ADD_POPUP)
            {
                Plane* plane = new Plane(ui->input1->text().toStdString(), ui->input2->text().toStdString(), ui->input3->text().toInt(), ui->input4->text().toInt());
                planesList.addPlane(plane);
            }
            else if (currentPopup == PopupType::UPDATE_POPUP)
            {
                strcpy(planesList.data[selectedRow]->idPlane, ui->input1->text().toStdString().c_str());
                strcpy(planesList.data[selectedRow]->type, ui->input2->text().toStdString().c_str());

                int r = ui->input3->text().toInt();
                int c = ui->input4->text().toInt();

                // Sử dụng qDebug() để kiểm tra giá trị r và c
                qDebug() << "Row (r):" << r;
                qDebug() << "Column (c):" << c;

                planesList.data[selectedRow]->row = r;
                planesList.data[selectedRow]->column = c;
                planesList.data[selectedRow]->seats = r * c;

                ui->input3->setFocus();
            }
            OnPlanesButtonClicked();
            SetStatusPopupWidget(false, PopupType::NON_POPUP);
        }
        break;
    case PageType::FLIGHT:
        if (currentPopup == PopupType::FIND_POPUP)
        {
            string date = ui->dateEdit->dateTime().toString("yyyy:MM:dd").toStdString();
            string location = ui->input3->text().toStdString();
            if (location.empty())
            {
                ui->warring_3->setText("Điểm đến trống");
                ui->warring_3->setVisible(true);
            }
            else
            {
                ui->warring_3->setVisible(false);
                SetStatusPopupWidget(false, PopupType::NON_POPUP);

                ui->tableInfo->clearContents();
                int i = 0;
                auto flight = flightManager.flightList;
                while (flight != nullptr)
                {
                    if (strcmp(flight->info.arrive, location.c_str()) == 0 && flight->info.compareDate(date) == 0)
                    {
                        ui->tableInfo->setItem(i, 0, new QTableWidgetItem(flight->info.idFlight));
                        ui->tableInfo->setItem(i, 1, new QTableWidgetItem(flight->info.idPlane));
                        ui->tableInfo->setItem(i, 2, new QTableWidgetItem(flight->info.date.formatStr().c_str()));
                        ui->tableInfo->setItem(i, 3, new QTableWidgetItem(flight->info.arrive));
                        string s;
                        switch (flight->info.status) {
                        case CANCLE_FLIGHT:
                            s = "Cancle";
                            break;
                        case HAVE_TICKET:
                            s = "Have ticket";
                            break;
                        case OUT_OF_TICKET:
                            s = "Out of ticket";
                            break;
                        case COMPLETE_FLIGHT:
                            s = "Complete";
                            break;
                        default:
                            s = "";
                            break;
                        }
                        if (flight->info.totalTicket == planesList.findPlaneByID(flight->info.idPlane)->seats)
                        {
                            s = "Out of ticket";
                        }
                        ui->tableInfo->setItem(i, 4, new QTableWidgetItem(s.c_str()));
                        string textSeats = to_string(flight->info.totalTicket) + "/" + to_string(planesList.findPlaneByID(flight->info.idPlane)->seats);
                        ui->tableInfo->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(textSeats)));
                        i++;
                    }
                    flight = flight->next;
                }
                ui->tableInfo->setRowCount(i);
            }
        }
        else if (FlightValidateInput())
        {
            string idF = ui->input1->text().toStdString();
            string idP = ui->idPlane->currentText().toStdString();
            string location = ui->input3->text().toStdString();
            int total = ui->input4->text().toInt();
            int seats = planesList.findPlaneByID(idP)->seats;
            Status status = ui->dateTimeEdit->dateTime() > QDateTime::currentDateTime() ? HAVE_TICKET : COMPLETE_FLIGHT;
            QString date = ui->dateTimeEdit->dateTime().toString("yyyy:MM:dd:hh:mm");

            if (currentPopup == PopupType::ADD_POPUP)
            {
                Flight flight = Flight(idF, idP, date.toStdString(), location, status, total);
                flightManager.insertAfter(flight);
            }
            else if (currentPopup == PopupType::UPDATE_POPUP)
            {
                auto flight = flightManager.findFlightByIdFlight(idF.c_str());
                strcpy(flight->info.idFlight, idF.c_str());
                strcpy(flight->info.idPlane, idP.c_str());
                flight->info.date = DateType(date.toStdString());
                strcpy(flight->info.arrive, location.c_str());
                flight->info.status = (Status)status;
                flight->info.totalTicket = total;
            }
            OnFlightsButtonClicked();
            SetStatusPopupWidget(false, PopupType::NON_POPUP);
        }
        break;
    case PageType::CUSTOMER:
        SetStatusPopupWidget(false, PopupType::NON_POPUP);
        break;
    case PageType::TICKET:
        if (currentPopup == PopupType::ADD_POPUP)
        {
            if (PassengerValidateInput())
            {
                string cccd = ui->input1->text().toStdString();
                string ho = ui->input3->text().toStdString();
                string ten = ui->input4->text().toStdString();
                int gender = ui->statusFlight->currentIndex();
                string idF = ui->idPlane->currentText().toStdString();

                auto flight = flightManager.findFlightByIdFlight(idF.c_str());
                flight->info.totalTicket++;
                flight->info.passengers.insert(make_pair(cccd, locationChoose));

                Passenger passenger = Passenger(cccd, ho, ten, gender);
                passengerManager.passengerList = passengerManager.addPassenger(passengerManager.passengerList, passenger);
                SetStatusPopupWidget(false, PopupType::NON_POPUP);
                ShowAlert("ĐẶT VÉ THÀNH CÔNG");
            }
        }
        else if (currentPopup == PopupType::UPDATE_POPUP)
        {
            string idF = ui->idPlane->currentText().toStdString();
            string idPass = ui->statusFlight->currentText().toStdString();
            auto flight = flightManager.findFlightByIdFlight(idF.c_str());
            if (flight->info.findLocationPassengerByID(idPass) == -1)
            {
                ShowAlert("LỖI: Khách hàng không có trong chuyến bay");
            }
            else
            {
                flight->info.removePassenger(idPass);
                ShowAlert("HỦY VÉ THÀNH CÔNG");
            }

            SetStatusPopupWidget(false, PopupType::NON_POPUP);
        }
        break;
    default:
        break;
    }
    SaveFile();
};

void MainWindow::OnTableWidgetCellClicked(int row, int column)
{
    if (currentPage == REPORT || currentPage == CUSTOMER)
    {
        return;
    }

    ui->removeButton->setVisible(true);
    ui->updateButton->setVisible(true);
    selectedRow = row;
};

void MainWindow::OnFindButtonClicked()
{
    currentPopup = PopupType::FIND_POPUP;
    SetStatusPopupWidget(false, PopupType::NON_POPUP);
    DisPlayPopup(PopupType::FIND_POPUP);
}

void MainWindow::focusOutEvent()
{
    ui->removeButton->setVisible(false);
    ui->updateButton->setVisible(false);
    ui->addButton->setVisible(false);

    ui->flightInfo->setVisible(false);
    ui->comboBox_1->setVisible(false);
    ui->comboBox_2->setVisible(false);
    ui->comboBox_3->setVisible(false);
    ui->dateEdit->setVisible(false);
    ui->gridLayoutWidget->setVisible(false);
    selectedRow = -1;
}

void MainWindow::DisPlayPopup(PopupType type)
{
    switch (type) {
    case PopupType::ADD_POPUP:
        SetStatusPopupWidget(true, PopupType::ADD_POPUP);
        break;
    case PopupType::UPDATE_POPUP:
        SetStatusPopupWidget(true, PopupType::UPDATE_POPUP);
        break;
    case PopupType::FIND_POPUP:
        SetStatusPopupWidget(true, PopupType::FIND_POPUP);
        break;
    case PopupType::DELETE_POPUP:
        break;
    default:
        break;
    }
};

void MainWindow::SetStatusPopupWidget(bool status, PopupType type)
{
    HideWarring();
    ClearDataInputBox();
    if (type == PopupType::NON_POPUP)
    {
        ui->input1->setVisible(false);
        ui->input2->setVisible(false);
        ui->input3->setVisible(false);
        ui->input4->setVisible(false);
        ui->okButton->setVisible(false);
        ui->cancelButton->setVisible(false);
        ui->popupBG->setVisible(false);
        ui->warring_3->setVisible(false);
        ui->warring_4->setVisible(false);
        ui->idPlane->setVisible(false);
        ui->dateTimeEdit->setVisible(false);
        ui->statusFlight->setVisible(false);
        ui->dateEdit->setVisible(false);
        ui->seatInput->setVisible(false);
        ui->searchSeatButton->setVisible(false);
        return;
    }

    QIntValidator *validator = new QIntValidator(1, 1000, this);

    switch (currentPage)
    {
    case PageType::PLANE:
        ui->input1->setVisible(status);
        ui->input1->setPlaceholderText("Nhập số hiệu...");
        ui->input2->setVisible(status);
        ui->input2->setPlaceholderText("Nhập loại máy bay...");
        ui->input3->setVisible(status);
        ui->input3->setPlaceholderText("Nhập số ghế hàng ngang...");
        ui->input4->setVisible(status);
        ui->input4->setPlaceholderText("Nhập số ghế hàng dọc...");

        ui->input3->setValidator(validator);
        ui->input4->setValidator(validator);

        ui->okButton->setVisible(status);
        ui->cancelButton->setVisible(status);
        ui->popupBG->setVisible(status);
        if (type == PopupType::UPDATE_POPUP)
        {
            QString id = ui->tableInfo->item(selectedRow, 0)->text();
            ui->input1->setText(id);
            ui->input2->setText(ui->tableInfo->item(selectedRow, 1)->text());
            int r = planesList.findPlaneByID(id.toStdString())->row;
            ui->input3->setText(QString::number(r));
            int c = planesList.findPlaneByID(id.toStdString())->column;
            ui->input4->setText(QString::number(c));
        }
        break;
    case PageType::FLIGHT:
        if (type == PopupType::FIND_POPUP)
        {
            ui->dateEdit->setVisible(status);
            ui->input3->setVisible(status);
            ui->popupBG->setVisible(status);
            ui->input3->setPlaceholderText("Nhập địa điểm...");
            ui->okButton->setVisible(status);
            ui->cancelButton->setVisible(status);
        }
        else
        {
            ui->input1->setVisible(status);
            ui->input1->setPlaceholderText("Nhập mã chuyến bay...");
            ui->input3->setVisible(status);
            ui->input3->setPlaceholderText("Nhập điểm đến...");
            ui->input3->setValidator(nullptr);
            // ui->input4->setVisible(status);
            // ui->input4->setPlaceholderText("Nhập số hành khách...");
            // ui->input4->setValidator(validator);

            ui->idPlane->setVisible(status);
            ui->idPlane->clear();
            for (int i = 0; i < planesList.size; ++i)
            {
                ui->idPlane->addItem(planesList.data[i]->idPlane);
            }
            ui->dateTimeEdit->setVisible(status);
            ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
            // ui->statusFlight->setVisible(status);
            // ui->statusFlight->clear();
            // ui->statusFlight->addItems({"Hủy chuyến", "Còn vé", "Hết vé", "Hoàn thành"});

            ui->okButton->setVisible(status);
            ui->cancelButton->setVisible(status);
            ui->popupBG->setVisible(status);
            if (type == PopupType::UPDATE_POPUP)
            {
                QString id = ui->tableInfo->item(selectedRow, 0)->text();

                ui->input1->setText(id);
            }
        }
        break;
    case PageType::CUSTOMER:
        break;
    case TICKET:
        if (type == PopupType::ADD_POPUP)
        {
            ui->input1->setVisible(status);
            ui->input1->setPlaceholderText("Nhập cccd...");
            ui->input3->setVisible(status);
            ui->input3->setPlaceholderText("Nhập họ...");
            ui->input3->setValidator(nullptr);
            ui->input4->setVisible(status);
            ui->input4->setPlaceholderText("Nhập tên...");
            ui->input4->setValidator(nullptr);

            ui->idPlane->setVisible(status);
            ui->idPlane->clear();
            auto p = flightManager.flightList;
            while (p != NULL)
            {
                ui->idPlane->addItem(p->info.idFlight);
                p = p->next;
            }

            ui->statusFlight->setVisible(status);
            ui->statusFlight->clear();
            ui->statusFlight->addItems({"Chọn giới tính " ,"Nam", "Nữ"});

            ui->okButton->setVisible(status);
            ui->cancelButton->setVisible(status);
            ui->popupBG->setVisible(status);
        }
        else if (type == PopupType::UPDATE_POPUP)
        {
            ui->idPlane->setVisible(status);
            ui->idPlane->clear();
            auto p = flightManager.flightList;
            while (p != NULL)
            {
                ui->idPlane->addItem(p->info.idFlight);
                p = p->next;
            }

            ui->statusFlight->setVisible(status);
            ui->statusFlight->clear();
            idPassengerList.clear();
            GetIDPassengerList(passengerManager.passengerList);
            for (int i = 0; i < idPassengerList.size(); ++i)
            {
                ui->statusFlight->addItem(idPassengerList[i]);
            }
            ui->okButton->setVisible(status);
            ui->cancelButton->setVisible(status);
            ui->popupBG->setVisible(status);
        }

        break;
    case REPORT:
        break;
    default:
        ui->input1->setVisible(false);
        ui->input2->setVisible(false);
        ui->input3->setVisible(false);
        ui->input4->setVisible(false);
        ui->okButton->setVisible(false);
        ui->cancelButton->setVisible(false);
        ui->popupBG->setVisible(false);
        ui->idPlane->setVisible(false);
        ui->dateTimeEdit->setVisible(false);
        ui->statusFlight->setVisible(false);

        break;
    }
};

void MainWindow::SetStatusButton(bool status)
{
    ui->addButton->setVisible(status);
    ui->addButton->setText("Thêm");
    ui->tableInfo->setVisible(status);
}




void MainWindow::CreateButtonGrid(int c, int r, map<string, int> passengerList)
{
    // Xóa tất cả các widget trong layout trước khi tạo lại
    while (QLayoutItem *item = ui->gridLayout->takeAt(0))
    {
        if (QWidget *widget = item->widget())
        {
            widget->deleteLater(); // Xóa widget
        }
        delete item; // Xóa QLayoutItem
    }

    vector<QPushButton*> buttonList; // Danh sách các nút ghế

    for (int i = 0; i < c * r; ++i)
    {
        // Tạo một nút mới đại diện cho một ghế
        QPushButton *button = new QPushButton();
        button->setFixedSize(30, 30); // Đặt kích thước cố định cho nút
        button->setStyleSheet("background-color: #b3b3b3; color: #333333;"); // Màu nền mặc định

        // Gắn sự kiện click cho nút
        connect(button, &QPushButton::clicked, [this, i]()
                {
                    locationChoose = i;
                    OnAddButtonClicked();
                });

        // Tính toán hàng và cột cho mỗi nút
        int row = i / c;
        int column = i % c;

        // Tạo nhãn (label) cho vị trí ghế, ví dụ "A1", "A2"...
        string locate = static_cast<char>(row + 'A') + std::to_string(column + 1);
        button->setText(QString::fromStdString(locate).toUpper()); // Đặt nhãn cho nút
        qDebug() << "Đã tạo ghế với mã:" << button->text(); // Gỡ lỗi - In mã ghế khi tạo

        // Thêm nút vào layout tại vị trí hàng và cột
        ui->gridLayout->addWidget(button, row, column);

        // Thêm nút vào danh sách
        buttonList.push_back(button);
    }

    // Đánh dấu các ghế đã được đặt
    for (auto it : passengerList)
    {
        if (it.second < buttonList.size()) // Kiểm tra chỉ số để tránh lỗi
        {
            buttonList[it.second]->setStyleSheet("background-color: red;"); // Đổi màu ghế đã đặt thành đỏ
            buttonList[it.second]->setEnabled(false); // Vô hiệu hóa nút ghế đã đặt
        }
    }
}

void MainWindow::onsearchSeatButtonclicked()
{
    // Lấy mã ghế từ QLineEdit và chuyển thành chữ hoa
    QString seatCode = ui->seatInput->text().trimmed().toUpper();
    qDebug() << "Tìm ghế với mã:" << seatCode; // In ra mã ghế cần tìm

    // Tìm kiếm trong danh sách tất cả các QPushButton
    bool found = false;
    for (auto button : this->findChildren<QPushButton*>())
    {
        qDebug() << "Đang kiểm tra ghế:" << button->text(); // Kiểm tra từng mã ghế có trong layout
        if (button->text().trimmed().toUpper() == seatCode)
        {
            // Nếu tìm thấy, đổi màu hoặc hiển thị gì đó để báo hiệu
            button->setStyleSheet("background-color: yellow; color: black;");
            found = true;
            break;
        }
    }

    // Hiển thị thông báo nếu không tìm thấy
    if (!found)
    {
        QMessageBox::information(this, "Tìm kiếm ghế", "Không tìm thấy ghế " + seatCode);
    }
}

void MainWindow::OnSearchPlaneButtonClicked()
{
    // Lấy số hiệu từ trường nhập liệu và chuyển thành chữ hoa
    QString searchKey = ui->searchInput->text().toUpper();
    ui->searchInput->setText(searchKey); // Cập nhật lại trường nhập liệu với chữ hoa

    // Kiểm tra nếu trống
    if (searchKey.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng nhập số hiệu máy bay cần tìm!");
        return;
    }

    // Tìm số hiệu máy bay trong danh sách
    int index = planesList.findPlane(searchKey.toStdString());
    if (index == -1) {
        QMessageBox::information(this, "Kết quả", "Không tìm thấy số hiệu máy bay!");
        return;
    }

    // Đánh dấu hàng tìm thấy với màu xanh lá nhạt
    QColor color(144, 238, 144, 178); // Màu xanh lá nhạt (RGBA)
    for (int col = 0; col < ui->tableInfo->columnCount(); col++) {
        if (!ui->tableInfo->item(index, col)) {
            ui->tableInfo->setItem(index, col, new QTableWidgetItem()); // Tạo item nếu chưa có
        }
        ui->tableInfo->item(index, col)->setBackground(color);
    }

    // Cuộn đến hàng được đánh dấu
    ui->tableInfo->scrollToItem(ui->tableInfo->item(index, 0));

    // Tự động xóa màu sau 5 giây
    QTimer::singleShot(5000, this, [this, index]() {
        for (int col = 0; col < ui->tableInfo->columnCount(); col++) {
            if (ui->tableInfo->item(index, col)) {
                ui->tableInfo->item(index, col)->setBackground(Qt::NoBrush); // Xóa màu nền
            }
        }
    });
}



void MainWindow::ShowAlert(QString content)
{
    ui->alertLabel->setStyleSheet("background-color: yellow; color: black; font-size: 10px; padding: 10px;");
    ui->alertLabel->setAlignment(Qt::AlignCenter);
    ui->alertLabel->raise();
    ui->alertLabel->setText(content);
    ui->alertLabel->setVisible(true);

    QTimer::singleShot(3000, this, [this]() {
        ui->alertLabel->setVisible(false);
    });
}

void MainWindow::SaveFile()
{
    planesList.writeFilePlane();
    flightManager.writeFileFlight();
    passengerManager.writeFilePassenger(passengerManager.passengerList);

}

void MainWindow::UpdateFlightTimesOfPlane()
{
    planesList.clearFlyTimes();
    auto temp = flightManager.flightList;

    while (temp != nullptr)
    {
        if (temp->info.status == COMPLETE_FLIGHT)
        {
            planesList.findPlaneByID(temp->info.idPlane)->flyTimes++;
        }
        temp = temp->next;
    }
}

void MainWindow::ClearDataInputBox()
{
    ui->input1->clear();
    ui->input2->clear();
    ui->input3->clear();
    ui->input4->clear();
}
