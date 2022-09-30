#include <QMainWindow>
#include <QStackedWidget>
#include <QScrollArea>
#include <QWidget>

// for reading json (nlohmann)
#include <nlohmann/json.hpp>
#include <fstream>
#include <variant>

class ToDo: public QMainWindow {
    Q_OBJECT

public:
    explicit ToDo(QWidget *parent = 0);
    
    void showManagePage();
    void showMainPage();

    void showAddDialog();
    void showEditDialog();
    void showDeleteDialog();
    void showSettingsDialog();

    nlohmann::json loadItems();
    nlohmann::json updateItem(int id, std::variant<std::string, bool> value);

private:
    QStackedWidget *stackedWidget;

    QWidget *mainPage;
    QScrollArea *mainPageList;
    
    QWidget *managePage;
    QScrollArea *managePageList;

    void resizeEvent(QResizeEvent *event) override;
};
