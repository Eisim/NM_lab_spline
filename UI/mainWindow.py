from PyQt5 import uic
from PyQt5.QtWidgets import QMainWindow, QTableWidgetItem, QLabel
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas, \
    NavigationToolbar2QT as NavigationToolbar
from matplotlib.figure import Figure

from UI.infoWindow import UI_infoWindow

import ctypes
import os
import time

from table_columns import columns, extra_info_rows

ui_file = './UI/MainWindow.ui'


def create_plot(parent):
    parent.fig = Figure(figsize=(parent.width() / 100, parent.height() / 100))
    parent.canvas = FigureCanvas(parent.fig)
    parent.plot = parent.fig.add_subplot()
    return parent.plot


class UI_mainWindow(QMainWindow):
    def __init__(self):

        if not os.path.isdir('data'):
            os.mkdir('data')# дириктория для записи файлов

        super(UI_mainWindow, self).__init__()
        uic.loadUi(ui_file, self)
        # определение значений по умолчанию

        # создание окон для графиков
        self.plt = create_plot(self.plot_widget_1) # Функция и сплайн на одном графике
        self.plt_PS = create_plot(self.plot_widget_2) # График первых производных функции и сплайна
        self.plt_QS = create_plot(self.plot_widget_3) # Погрешности на одном графике

        # присвоение мест для окон
        self.plot_widget_1.canvas.setParent(self.plot_widget_1)
        self.plot_widget_2.canvas.setParent(self.plot_widget_2)
        self.plot_widget_3.canvas.setParent(self.plot_widget_3)

        self.tabWidget.currentChanged.connect(
            self.toolBar_changing)  # задание функционала. В данной строке: Меняет тулбар при переходе на другую вклвдку
        self.plot_toolBar = NavigationToolbar(self.plot_widget_1.canvas, self)

        self.addToolBar(self.plot_toolBar)  # создание тулбара

        self.plot_button.clicked.connect(
            self.plotting)  # задание функционала. В данной строке: построение графика при нажатии на кнопку "Построить"
        self.delete_plot.clicked.connect(
            self.clear_plots)  # задание функционала. В данной строке: очистка окон от ВСЕХ графиков (чистит все окна(графики и таблицу))

        # Названия осей
        self.plot_widget_1.plot.set_xlabel("x")
        self.plot_widget_1.plot.set_ylabel("Значение")

        self.plot_widget_2.plot.set_xlabel("x")
        self.plot_widget_2.plot.set_ylabel("Значение")

        self.plot_widget_3.plot.set_xlabel("x")
        self.plot_widget_3.plot.set_ylabel("Значение")

        # настройка включения второго окна
        self.info_button.triggered.connect(lambda: self.info_window("my_info.pdf"))

    def info_window(self,file_name):
        self.i_window = QMainWindow()
        self.i_window.ui = UI_infoWindow(file_name)
        self.i_window.ui.show()

    def clear_plots(self):
        self.plt.cla()
        self.plt_PS.cla()
        self.plot_widget_1.canvas.draw()  # обновление окна
        self.plot_widget_2.canvas.draw()
        self.plot_widget_3.canvas.draw()

        self.clear_exrta_info_table()
        self.clear_table(self.info_table)
        self.clear_table(self.info_table_2)
        # self.clear_table(self.info_table_V_dot)

        # Названия осей
        self.plot_widget_1.plot.set_xlabel("x")
        self.plot_widget_1.plot.set_ylabel("Значение")

        self.plot_widget_2.plot.set_xlabel("x")
        self.plot_widget_2.plot.set_ylabel("Значение")

        self.plot_widget_3.plot.set_xlabel("x")
        self.plot_widget_3.plot.set_ylabel("Значение")

    def toolBar_changing(self, index):  # изменение привязки тулбара
        self.removeToolBar(self.plot_toolBar)
        if index == 0:  # тулбал для вкладки # Функция и сплайн на одном графике
            self.plot_toolBar = NavigationToolbar(self.plot_widget_1.canvas, self)
        elif index == 1:  # тулбар для вкладки # График первых производных функции и сплайна
            self.plot_toolBar = NavigationToolbar(self.plot_widget_2.canvas, self)
        elif index == 2: # тулбар для вкладки # Погрешности на одном графике
            self.plot_toolBar = NavigationToolbar(self.plot_widget_3.canvas, self)
        self.addToolBar(self.plot_toolBar)

    def file_to_table(self, file_name):  # из str делает list(list(str))
        if len(file_name.split('.')) == 1:
            file_name += '.txt'
        table = []
        with open(file_name, 'r') as f:
            for line in f:
                table.append(line.split(' '))
        return table

    def clear_exrta_info_table(self):
        while self.extra_info_layout.count():
            item = self.extra_info_layout.takeAt(0)
            widget = item.widget()
            if widget is not None:
                widget.deleteLater()

    def update_extra_info_table(self, task_index, table):
        self.clear_exrta_info_table()

        table = table[0]
        i = 0
        cur_table = extra_info_rows[task_index]
        for elem in table:
            cur_text = f"{cur_table[i]} {elem}"
            self.extra_info_layout.addWidget(QLabel(cur_text, self))
            i += 1

    def plotting(self):
        # lib_dir = os.path.join(os.curdir, "libNM1_lib.dll")
        lib_dir = os.path.join(os.curdir, 'dll', 'Release', "labSplines_lib.dll") # Что запускаем
        lib = ctypes.windll.LoadLibrary(lib_dir)



        # Начальные значения системы (w,s)
        n = int(self.get_num_uzlov()) # Количество УЧАСТКОВ

        # Задача
        task = self.get_task() # Тестовая | Основная | Осцилляция
        # Функция из 3-х для основной задачи (см. видос Капкаева)
        task_func = self.get_task_main_func() # f1 | f2 | f3

        my_func =lib.write_to_files
        # if task == 0:
        #
        #     # my_func = lib.run_progonka_test
        #     file_name_1 = "test_method_1"
        #     # file_name_for_V_dot = "rigid_syst_data_s"
        #
        # else:
        #     # my_func = lib.run_main_method_2_const_step
        #     # file_name = "main_method_2_1_const_step_v"
        #     # file_name_for_V_dot = "main_method_2_1_const_step_v_dot"
        #     # file_name_extra_info = 'main_method_2_2_const_step'
        #
        #     # my_func = lib.run_progonka_main
        #     file_name = "main_method_1"

        file_name_1 = "table_1"
        file_name_2 = "table_2"

        file_name_3 = "for_plots"

        file_name_extra_info = 'spravka'

        my_func.argtypes = [ctypes.c_int,ctypes.c_int,#n, N
                            ctypes.c_double,ctypes.c_double,#M1,M2
                            ctypes.c_double,ctypes.c_double,#a,b
                            ctypes.c_int, # task_type
                            ctypes.c_int,# task_function
                            ]
        my_func.restype = ctypes.c_void_p
        my_func(n, 2*n, 0, 0, 0.1,0.9,0)

        # self.clear_table(self.info_table_V_dot)


        # self.set_table(self.info_table_V_dot, self.file_to_table(file_name_for_V_dot), file_name_for_V_dot)

        self.clear_table(self.info_table)
        table_1 = self.file_to_table(file_name_1)  # Парсинг файла в табличный вид (тип ячейки:str)

        self.set_table(self.info_table, table_1, file_name_1)  # заполнение таблицы(вкладка "Таблица")

        self.clear_table(self.info_table_2)

        table_2 = self.file_to_table(file_name_2)  # Парсинг файла в табличный вид (тип ячейки:str)

        self.set_table(self.info_table_2, table_2, file_name_2)  # заполнение таблицы(вкладка "Таблица")

        table_extra_info = self.file_to_table(file_name_extra_info)
        self.update_extra_info_table(file_name_extra_info,table_extra_info)  # заполнение вспомогательной информации(правый нижний угол)

        table_3 = self.file_to_table(file_name_3)  # Парсинг файла в табличный вид (тип ячейки:str) # Здесь графики

        X_arr = [float(row[0]) for row in table_3]
        F_arr = [float(row[1]) for row in table_3]
        S_arr = [float(row[2]) for row in table_3]
        Diff_arr = [float(row[3]) for row in table_3]

        F_dot_arr = [float(row[4]) for row in table_3]
        S_dot_arr = [float(row[5]) for row in table_3]
        Diff_dot_arr = [float(row[6]) for row in table_3]

        F_ddot_arr = [float(row[7]) for row in table_3]
        S_ddot_arr = [float(row[8]) for row in table_3]
        Diff_ddot_arr = [float(row[9]) for row in table_3]

        self.plt.plot(X_arr,F_arr,label = "Функция")
        self.plt.plot(X_arr, S_arr, label="Сплайн")
        self.plt.set_xlim(auto=True)
        self.plt.set_ylim(auto=True)
        self.plt.legend(loc="upper left")

        self.plt_PS.plot(X_arr, F_dot_arr, label="Производная функции")
        self.plt_PS.plot(X_arr, S_dot_arr, label="Производная сплайна")
        self.plt_PS.set_xlim(auto=True)
        self.plt_PS.set_ylim(auto=True)
        self.plt_PS.legend(loc="upper left")

        self.plt_QS.plot(X_arr, Diff_arr, label="Погрешность сплайна")
        self.plt_QS.plot(X_arr, Diff_dot_arr, label="Погрешность первой производной")
        self.plt_QS.plot(X_arr, Diff_ddot_arr, label="Погрешность второй производной")
        self.plt_QS.set_xlim(auto=True)
        self.plt_QS.set_ylim(auto=True)
        self.plt_QS.legend(loc="upper left")

        # self.plt_PS.plot(X_arr, Diff_arr, label="Погрешность")
        # self.plt_PS.legend(loc="upper left")
        # if task == 0:
        #     labels = ["Аналитическое решение","Численное решение"]
        # else:
        #     labels = ["Численное решение","Численное решение удв. шагом"]
        # self.plt.plot(X_arr, U_arr, label=labels[0])
        # # self.plt.scatter(X_start, w_0,label="Старт. точка (V1)")  # scatter - построение точечного графика. В данном случае просто ставит точку (x0,u0)
        #
        # self.plt.plot(X_arr, V_arr, label=labels[1])
        # self.plt.scatter(X_start, s_0,label="Старт. точка (V2)")


        # self.plt.legend(loc="upper left")  # legend - задание окна легенд

        self.plot_widget_1.canvas.draw()
        self.plot_widget_2.canvas.draw()
        self.plot_widget_3.canvas.draw()

    # def get_X_start(self):
    #     return self.X_start.text()
    #
    # def get_X_end(self):
    #     return self.X_end.text()

    # def get_U0(self):
    #     return self.U_X0.text()
    #
    # def get_DU0(self):
    #     return self.DU_X0.text()

    # def get_start_step(self):
    #     return self.step_start.text()
    #
    # def get_step_control(self):
    #     return self.step_control.text()
    #
    # def get_border_control(self):
    #     return self.border_control.text()

    # def get_task(self):
    #     return self.task_selection_box.currentIndex(), self.task_selection_box.currentText()

    # def get_step_mode(self):
    #     return self.step_mode.isChecked()

    def set_row(self, table, row):
        max_row_index = table.rowCount()
        table.insertRow(max_row_index)  # создание строки
        for i in range(len(row)):
            table.setItem(max_row_index, i, QTableWidgetItem(str(row[i])))  # заполнение элементами

    def set_columns(self, table, task_index):
        cols = columns[task_index]
        table.setColumnCount(len(cols))  # создание пустых колонок, в количестве len(cols) штук
        table.setHorizontalHeaderLabels(cols)  # присвоение имен для колонок

    def set_table(self, table, data, task_index):
        self.set_columns(table, task_index)
        for row in data:
            self.set_row(table, row)

    def clear_table(self, table):
        while (table.rowCount() > 0):
            table.removeRow(0)

    # def get_num_max_iter(self):
    #     return self.max_num_iter.text()

    def get_num_uzlov(self):
        return self.input_n.text()

    def get_task(self):
        return self.comboBox.currentIndex()

    def get_task_main_func(self):
        return self.comboBox_functions.currentIndex()