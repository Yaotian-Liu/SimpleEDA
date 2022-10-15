/**
 * @file MainWindow.h
 * @author Yaotian Liu
 * @brief Header file of MainWindow
 * @date 2022-10-10
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

class QAction;
class QMenu;
class QToolBar;
class QTextEdit;
class QWidget;
class QTextStream;

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();
    void CreateMenus();
    void CreateActions();
    void CreateToolBars();

  public slots:
    void SlotNewFile();
    void SlotOpenFile();
    void SlotSaveFile();

    void SlotParser();

  private:
    /// @brief file
    QMenu* file_menu;

    QToolBar* file_tool;

    QAction* action_new_file;
    QAction* action_open_file;
    QAction* action_save_file;

    /// @brief parser
    QToolBar* parser_tool;

    QAction* action_parser;

    QTextEdit* text;

    QString file_name = "./";
};

#endif