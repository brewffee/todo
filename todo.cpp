#include "todo.h"

#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QScrollBar>

#include <QMouseEvent>

#include <iostream>

// TO DO
//
// Add item dialog
// Item edit dialog
// Delete confirmation dialog
// Settings dialog
// Settings save/load
// Manage page
// Better styling

ToDo::ToDo(QWidget *parent) : QMainWindow(parent) {
    setMinimumSize(400, 400);
    setWindowTitle("ToDo");
    setStyleSheet("background-color: #f7f7f7;");
    setContentsMargins(6, 0, 6, 6);

    nlohmann::json items = loadItems();

    stackedWidget = new QStackedWidget(this);

    // stackedWidget {
    //   QWidget: mainPage (none)
    //     QVBoxLayout: mainLayout (none)
    //       QScrollArea: scrollArea -> setResizable(true)
    //         QWidget: listWidget (none)
    //           QVBoxLayout: listLayout (none)
    //             QWidget: itemWidget (none)
    //               QHBoxLayout: itemLayout (none)

    QWidget *mainPage = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainPage);

    // Search box and filter area
    // [ Search box ] [ filter button ]
    // [ Active tags   -> -> -> ]
    // [ Content ]
    QVBoxLayout *searchLayout = new QVBoxLayout();
    QHBoxLayout *searchBarLayout = new QHBoxLayout();
    QHBoxLayout *tagLayout = new QHBoxLayout();

    // create the search bar and button
    QLineEdit *searchBar = new QLineEdit();
    

    QScrollArea *scrollArea = new QScrollArea(mainPage);
    scrollArea->setWidgetResizable(true);
    scrollArea->setContentsMargins(0, 0, 0, 0);

    // remove padding from scrollArea
    scrollArea->setStyleSheet("QScrollArea { border: none; }");

    // get the scrollbar
    QScrollBar *scrollBar = scrollArea->verticalScrollBar();

    // style the scrollbar
    scrollBar->setStyleSheet(
        "QScrollBar { border: none; background: transparent; width: 8px; }"
        "QScrollBar::handle { background: rgba(0,0,0,0.1); border: none; border-radius: 4px; }"
        "QScrollBar::handle:hover { background: rgba(0,0,0,0.125); }"
        "QScrollBar::handle:pressed { background: rgba(0,0,0,0.15); }"
        "QScrollBar::add-line, QScrollBar::sub-line { background: none; }"
        "QScrollBar::add-page, QScrollBar::sub-page { background: none; }"
    );
    
    QWidget *listWidget = new QWidget(scrollArea);
    QVBoxLayout *listLayout = new QVBoxLayout(listWidget);

    if (items.size() == 0) {
        // Build a whole page out of this later
        QLabel *noItemsLabel = new QLabel("You're all caught up!", mainPageList);
        noItemsLabel -> setStyleSheet("color: #999999; font-size: 24px;");

        listLayout -> addWidget(noItemsLabel, 0, Qt::AlignCenter);
    }

    // else we can show the items
    else {
        for (int i = 0; i < items.size(); i++) {
            QWidget *itemWidget = new QWidget();
            QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);

            itemWidget -> setContentsMargins(0, 0, 0, 0);
            itemLayout -> setContentsMargins(0, 0, 0, 0);

            itemWidget -> setObjectName("itemWidget"); // use classes later
            itemWidget -> setStyleSheet(
                "QWidget#itemWidget {"
                    "background-color: #fff;"
                    "border-radius: 8px;"
                    "padding: 12px 24px;"
                "}"
                "QWidget#itemWidget:hover {"
                    "background-color: #fcfcfc;"
                    "border: 1px solid #ccc;"
                "}"
            );
            itemWidget -> setCursor(Qt::PointingHandCursor);

            itemWidget -> setMinimumHeight(48);
            itemWidget -> setContentsMargins(12,6,12,6);


            // Get the name and if it's done
            std::string name = items[i]["name"];
            bool done = items[i]["done"];
            int id = items[i]["id"];

            // Create the item
            QLabel *itemLabel = new QLabel(name.c_str());
            itemLabel -> setStyleSheet("background-color: transparent;");
            QCheckBox *itemCheckbox = new QCheckBox();
            // set a property to store the id
            itemCheckbox -> setProperty("id" , id);
            itemCheckbox -> setObjectName /* item name */ (name.c_str());

            if (done) itemCheckbox -> setChecked(true);

            itemLayout -> addWidget(itemLabel, 0, Qt::AlignLeft);
            // itemLayout -> addStretch();
            itemLayout -> addWidget(itemCheckbox, 0, Qt::AlignRight);

            // set the layout
            itemWidget -> setLayout(itemLayout);

            
            // List of widgets go inside of a Vbox
            listLayout->addWidget(itemWidget);
        }
    }

    // place the scroll area inside of the main page
    scrollArea -> setWidget(listWidget);
    mainLayout -> addWidget(scrollArea);
    
    QPushButton *button = new QPushButton("Add item");
    button -> setContentsMargins(6,6,6,6);
    button  -> setObjectName("addItem");
    button -> setStyleSheet(
        "QWidget#addItem:hover { background-color: #fcfcfc }"
        "QWidget#addItem {"
            "background-color: #fff;"
            "border-radius: 8px;"
            "padding: 6px;"
        "}"
    );
    mainLayout -> addWidget(button);

    // Connect the button
    connect(button, &QPushButton::clicked, this, &ToDo::showManagePage);

    // Manage page
    managePage = new QWidget(stackedWidget);
    managePageList = new QScrollArea(managePage);

    QVBoxLayout *manageLayout = new QVBoxLayout(managePage);
    QVBoxLayout *manageListLayout = new QVBoxLayout();

    QPushButton *backButton = new QPushButton("Back");
    manageLayout -> addWidget(backButton);

    // Connect the button
    connect(backButton, &QPushButton::clicked, this, &ToDo::showMainPage);

    // Add the pages to the stacked widget
    stackedWidget -> addWidget(mainPage);
    stackedWidget -> addWidget(managePage);



    // Set the stacked widget as the central widget
    setCentralWidget(stackedWidget);
    showMainPage();
}

void ToDo::showManagePage() { stackedWidget -> setCurrentWidget(managePage); }
void ToDo::showMainPage() { stackedWidget -> setCurrentWidget(mainPage); }

void ToDo::showAddDialog() {

}

nlohmann::json ToDo::loadItems() {
    std::fstream file = std::fstream(getenv("HOME") + std::string("/.todo.json"), std::ios::in);

    // if the file doesnt exist, create it 
    if (!file.is_open()) {
        file = std::fstream(getenv("HOME") + std::string("/.todo.json"), std::ios::out);
        file << "{ \"items\": [] }" << std::endl;
        file.close();
        file = std::fstream(getenv("HOME") + std::string("/.todo.json"), std::ios::in);
    }

    // read contents
    std::string content(
        (std::istreambuf_iterator<char>(file)), 
        std::istreambuf_iterator<char>()
    );

    // attempt to parse content
    nlohmann::json j;

    try {
        j = nlohmann::json::parse(content);
    } catch (nlohmann::json::parse_error) {
        std::cout << "Malformed json :(" << std::endl;
    }

    // File has strict formatting rule:
    // {"items":[{"name": "value", "done": true, "id": 0}]}
    //
    // if the data doesn't match up, inform the user and exit (do other behav. later)
    if (!j.contains("items") || !j["items"].is_array()) {
        std::cout << "Items array missing or is of wrong type" << std::endl;
        exit(1);
    } else {
        // if items are intact, also check the existence or type of every item
        for (int i = 0; i < j["items"].size(); i++) {
            if (!j["items"][i].contains("name") || !j["items"][i].contains("done")) {
                std::cout << "name or done prop missing" << std::endl;
                exit(1);
            } else if (!j["items"][i]["name"].is_string() || !j["items"][i]["done"].is_boolean()) {
                std::cout << "name or done prop is of wrong type" << std::endl;
                exit(1);
            } else if (!j["items"][i].contains("id") || !j["items"][i]["id"].is_number_integer()) {
                std::cout << "id prop missing or is of wrong type" << std::endl;
                exit(1);
            }
        }

        // check for more later
    }

    return j["items"];
}

nlohmann::json updateItem(int id, std::variant<std::string, bool> value) {
    std::fstream file = std::fstream(getenv("HOME") + std::string("/.todo.json"), std::ios::in);

    // file already exists (loadItems() does this before calling this function)
    // read contents
    std::string content(
        (std::istreambuf_iterator<char>(file)), 
        std::istreambuf_iterator<char>()
    );

    // content will already be valid (loadItems() throws out bad json)
    nlohmann::json j = nlohmann::json::parse(content);

    // find the item by id
    for (int i = 0; i < j["items"].size(); i++) {
        if (j["items"][i]["id"] == id) {
            // if the value is a string, update the name
            if (std::holds_alternative<std::string>(value)) {
                j["items"][i]["name"] = std::get<std::string>(value);
            } else if (std::holds_alternative<bool>(value)) {
                j["items"][i]["done"] = std::get<bool>(value);
            }
        }
    }

    // write the new json to the file
    file.close();

    file = std::fstream(getenv("HOME") + std::string("/.todo.json"), std::ios::out);
    file << j.dump(4) << std::endl;
    file.close();

    return j;
}

// above declaration is needed for this function to work outside of the class
nlohmann::json ToDo::updateItem(int id, std::variant<std::string, bool> value) {
    return ::updateItem(id, value); // refs the global function
}

// mouse event filter
void QWidget::mousePressEvent(QMouseEvent *event) {
    // check name first
    if (objectName() == "itemWidget") {
        if (event -> button() != 2) {
            // get child checkbox 
            QCheckBox *checkbox = findChild<QCheckBox *>();
            checkbox -> setChecked(!checkbox -> isChecked());
        } else {
            // context menu
            std::cout << "opening context menu for item" << std::endl;
        }
    }
}

// also override when a checkbox's value changes
void QCheckBox::stateChanged(int state) {
    std::cout << objectName().toStdString() << " -> " << bool (state) << std::endl;

    // get id property and update the item
    int id = property("id").toInt();
    updateItem(id, bool (state));
}

// override ToDo's resize event
void ToDo::resizeEvent(QResizeEvent *event) {
    // get the width of the window
    int width = event -> size().width();

    // when the width exceeds 720px, alert the console
    // layout change will happen here
    if (width > 720) {
        std::cout << "width is greater than 720px! application is eligible for large view" << std::endl;
    } else {
        std::cout << "small view" << std::endl;
    }
}