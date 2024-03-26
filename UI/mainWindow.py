from PyQt5 import uic
from PyQt5.QtWidgets import QMainWindow, QTableWidgetItem, QLabel
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas, \
    NavigationToolbar2QT as NavigationToolbar
from matplotlib.figure import Figure

from UI.infoWindow import UI_infoWindow

import ctypes
import os
import time

from table_columns import columns, extra_info_rows, task_standart_params

ui_file = './UI/MainWindow.ui'


def create_plot(parent):
    parent.fig = Figure(figsize=(parent.width() / 100, parent.height() / 100))
    parent.canvas = FigureCanvas(parent.fig)
    parent.plot = parent.fig.add_subplot()
    return parent.plot


class UI_mainWindow(QMainWindow):
    def __init__(self):
        super(UI_mainWindow, self).__init__()
        uic.loadUi(ui_file, self)

        # создание окон для графиков
        self.plt = create_plot(self.plot_widget_1)  # Функция и сплайн на одном графике
        self.plt_PS = create_plot(self.plot_widget_2)  # График первых производных функции и сплайна
        self.plt_QS = create_plot(self.plot_widget_3)  # Погрешности на одном графике
        # присвоение мест для окон
        self.plot_widget_1.canvas.setParent(self.plot_widget_1)
        self.plot_widget_2.canvas.setParent(self.plot_widget_2)
        self.plot_widget_3.canvas.setParent(self.plot_widget_3)

        self.tabWidget.currentChanged.connect(
            self.toolBar_changing)  # задание функционала. В данной строке: Меняет тулбар при переходе на другую вклвдку
        self.plot_toolBar = NavigationToolbar(self.plot_widget_1.canvas, self)

        self.addToolBar(self.plot_toolBar)  # создание тулбара

        #функционал кнопок
        self.plot_button.clicked.connect(
            self.plotting)  # задание функционала. В данной строке: построение графика при нажатии на кнопку "Построить"
        self.delete_plot.clicked.connect(
            self.clear_plots)  # задание функционала. В данной строке: очистка окон от ВСЕХ графиков (чистит все окна(графики и таблицу))
        #функционал списков
        self.task_type.currentTextChanged.connect(self.standart_params)
        self.task_num.currentTextChanged.connect(self.standart_params)
        self.standart_params()
        self.task_index = 0
        # Названия осей
        self.plot_widget_1.plot.set_xlabel("x")
        self.plot_widget_1.plot.set_ylabel("Значение")

        self.plot_widget_2.plot.set_xlabel("x")
        self.plot_widget_2.plot.set_ylabel("Значение")

        self.plot_widget_3.plot.set_xlabel("x")
        self.plot_widget_3.plot.set_ylabel("Значение")

        # настройка включения второго окна
        self.info_button.triggered.connect(lambda: self.info_window("my_info.pdf"))

    def standart_params(self):
        task_type = self.task_type.currentIndex()
        task_num = self.task_num.currentIndex()
        self.task_index =0 # Индекс задачи в массиве
        if task_type == 0:
            self.task_index = 0
        elif task_type == 1:
            self.task_index = task_num + 1
        elif task_type == 2:
            self.task_index = task_num + 4
        a, b, M1, M2 = task_standart_params[self.task_index]
        self.input_a.setText(str(a))
        self.input_b.setText(str(b))
        self.input_mu_0.setText(str(M1))
        self.input_mu_1.setText(str(M2))

    def info_window(self, file_name):
        self.i_window = QMainWindow()
        self.i_window.ui = UI_infoWindow(file_name)
        self.i_window.ui.show()

    def clear_plots(self):
        self.clear_plot(self.plot_widget_1)
        self.clear_plot(self.plot_widget_2)
        self.clear_plot(self.plot_widget_3)
        self.clear_table(self.info_table)
        self.clear_table(self.info_table_2)
    def clear_plot(self, cur_plot_widget):
        cur_plot_widget.plot.cla()
        cur_plot_widget.canvas.draw()  # обновление окна

        # Названия осей
        cur_plot_widget.plot.set_xlabel("x")
        cur_plot_widget.plot.set_ylabel("Значение")
        cur_plot_widget.canvas.draw()

    def toolBar_changing(self, index):  # изменение привязки тулбара
        self.removeToolBar(self.plot_toolBar)
        if index == 0:  # тулбал для вкладки # Функция и сплайн на одном графике
            self.plot_toolBar = NavigationToolbar(self.plot_widget_1.canvas, self)
        elif index == 1:  # тулбар для вкладки # График первых производных функции и сплайна
            self.plot_toolBar = NavigationToolbar(self.plot_widget_2.canvas, self)
        elif index == 2:  # тулбар для вкладки # Погрешности на одном графике
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
        lib_dir = os.path.join(os.curdir, 'dll', 'Release', "labSplines_lib.dll")  # Что запускаем
        lib = ctypes.windll.LoadLibrary(lib_dir)


        file_name_1 = "table_1"
        file_name_2 = "table_2"

        file_name_3 = "for_plots"

        file_name_extra_info = 'spravka'

        my_func = lib.write_to_files
        my_func.argtypes = [ctypes.c_int, ctypes.c_int,  # n, N
                            ctypes.c_double, ctypes.c_double,  # M1,M2
                            ctypes.c_double, ctypes.c_double,  # a,b
                            ctypes.c_int,  # task_type
                            ctypes.c_int,  # task_function
                            ]
        my_func.restype = ctypes.c_void_p

        n = int(self.input_n.text())
        N = int(self.input_N.text())
        a = float(self.input_a.text())
        b = float(self.input_b.text())
        M1 = float(self.input_mu_0.text())
        M2 = float(self.input_mu_1.text())
        task_type = int(self.task_type.currentIndex())
        task_num = int(self.task_num.currentIndex())



        my_func(n, N, M1, M2, a, b, task_type, task_num)

        # self.clear_table(self.info_table_V_dot)

        # self.set_table(self.info_table_V_dot, self.file_to_table(file_name_for_V_dot), file_name_for_V_dot)

        self.clear_table(self.info_table)
        table_1 = self.file_to_table(file_name_1)  # Парсинг файла в табличный вид (тип ячейки:str)

        self.set_table(self.info_table, table_1, file_name_1)  # заполнение таблицы(вкладка "Таблица")

        self.clear_table(self.info_table_2)

        table_2 = self.file_to_table(file_name_2)  # Парсинг файла в табличный вид (тип ячейки:str)

        self.set_table(self.info_table_2, table_2, file_name_2)  # заполнение таблицы(вкладка "Таблица")

        table_extra_info = self.file_to_table(file_name_extra_info)
        self.update_extra_info_table(file_name_extra_info,
                                     table_extra_info)  # заполнение вспомогательной информации(правый нижний угол)

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

        self.plt.plot(X_arr, F_arr, label="Функция")
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
