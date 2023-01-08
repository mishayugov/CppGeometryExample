#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <imgui-SFML.h>
#include <imgui.h>
#include <fstream>

#include <bits/stdc++.h>

// первое множество
static const int SET_1 = 0;
// второе множество
static const int SET_2 = 1;
// пересечение множеств
static const int SET_CROSSED = 2;
// разность множеств
static const int SET_SINGLE = 3;

// Ширина окна
static const int WINDOW_SIZE_X = 800;
// Высота окна
static const int WINDOW_SIZE_Y = 800;

// путь к файлу вывода
static const char OUTPUT_PATH[255] = "C:/Users/dimay/CLionProjects/out.txt";
// путь к файлу ввода
static const char INPUT_PATH[255] = "C:/Users/dimay/CLionProjects/in.txt";

// круг
struct Circle {
    // положение центра
    sf::Vector2i pos;
    // положение точки на окружности
    sf::Vector2i pos2;

    // конструктор
    Circle(const sf::Vector2i &pos, const sf::Vector2i &pos2) : pos(pos), pos2(pos2) {
    }
};

// динамический список кругов
std::vector<Circle> circles;

// угол
struct Angle {
    // положение центра
    sf::Vector2i pos1;
    // положение твух точек
    sf::Vector2i pos2;
    sf::Vector2i pos3;

    // конструктор
    Angle(const sf::Vector2i &pos1, const sf::Vector2i &pos2,const sf::Vector2i &pos3) : pos1(pos1), pos2(pos2),pos3(pos3) {
    }
};

// динамический список углов
std::vector<Angle> angles;


// цвет фона
static sf::Color bgColor;
// значение цвета по умолчанию
float color[3] = {0.12f, 0.12f, 0.13f};

// буфер, хранящий координаты последней добавленной точки правой кнопкой
int fAddPosBufR[2] = {-1, 0};
// буфер, хранящий координаты предпоследней добавленной точки правой кнопкой
int sAddPosBufR[2] = {-1, 0};
// буфер, хранящий координаты предпредпоследней добавленной точки правой кнопкой
int tAddPosBufR[2] = {-1, 0};
// буфер, хранящий координаты предпредпоследней добавленной точки левой кнопкой
int fAddPosBufL[2] = {-1, 0};
// буфер, хранящий координаты предпредпоследней добавленной точки левой кнопкой
int sAddPosBufL[2] = {-1, 0};

// буфер кол-ва случайных точек
int lastRandoCntBuf[1] = {10};


// задать цвет фона по вещественному массиву компонент
static void setColor(float *pDouble) {
    bgColor.r = static_cast<sf::Uint8>(pDouble[0] * 255.f);
    bgColor.g = static_cast<sf::Uint8>(pDouble[1] * 255.f);
    bgColor.b = static_cast<sf::Uint8>(pDouble[2] * 255.f);
}



// запись в файл
void saveToFile() {
    // открываем поток данных для записи в файл
    std::ofstream output(OUTPUT_PATH);

    // перебираем окружности
    for (auto circle: circles) {
        output << 0<<" "<<circle.pos.x << " " << circle.pos.y << " " << circle.pos2.x <<" "<<circle.pos2.y <<std::endl;
    }
    // перебираем углы
    for (auto angle: angles) {
        output << 1<<" "<<angle.pos1.x << " " << angle.pos1.y << " " << angle.pos2.x << " " << angle.pos2.y<< " "<< angle.pos3.x << " " << angle.pos3.y << std::endl;
    }
    // закрываем
    output.close();
}

// решение задачи
void solve() {
}

// загрузка из файла
void loadFromFile() {
    // открываем поток данных для чтения из файла
    std::ifstream input(INPUT_PATH);
    // очищаем массив точек
    circles.clear();
    angles.clear();
    // пока не достигнут конец файла
    while (!input.eof()) {
        int x, y, rad, L,x1,x2,x3,y1,y2,y3;
        input >> L;
        if (L == 0) {
            input >> x; // читаем x координату центра
            input >> y; // читаем y координату центра
            input >> x1; // читаем x координату точки на окружности
            input>>y1; // читаем y координату точки на окружности
            // добавляем в динамический массив окружность на основе прочитанных данных
            circles.emplace_back(Circle(sf::Vector2i(x, y), sf::Vector2i(x1, y1)));
        }
        if (L != 0) {
            input >> x1;
            input >> y1;
            input >> x2;
            input >> y2;
            input >> x3;
            input >> y3;
            // добавляем в динамический массив угол на основе прочитанных данных
            angles.emplace_back(Angle(sf::Vector2i(x1, y1), sf::Vector2i(x2, y2), sf::Vector2i(x3, y3)));
        }
    }
    // закрываем файл
    input.close();
}

// рисование параметров цвета
void ShowBackgroundSetting() {
    // если не раскрыта панель `Background`
    if (!ImGui::CollapsingHeader("Background"))
        // заканчиваем выполнение
        return;

    // Инструмент выбора цвета
    if (ImGui::ColorEdit3("Background color", color)) {
        // код вызывается при изменении значения
        // задаём цвет фона
        setColor(color);
    }
}

// сумма векторов
static sf::Vector2i sum(sf::Vector2i a, sf::Vector2i b) {
    return {a.x + b.x, a.y + b.y};
}

// разность векторов
static sf::Vector2i subtract(sf::Vector2i a, sf::Vector2i b) {
    return {a.x - b.x, a.y - b.y};
}

// умножение вектора на число
static sf::Vector2f mul(sf::Vector2f a, float b) {
    return {a.x * b, a.y * b};
}

// преобразование целочисленного вектора в вещественный
static sf::Vector2f toFloat(sf::Vector2i v) {
    return {(float)v.x, (float)v.y};
}

// преобразование вещественного вектора в целочисленный
static sf::Vector2i toInt(sf::Vector2f v) {
    return {(int) v.x, (int) v.y};
}

// рисование задачи на невидимом окне во всё окно приложения
void RenderTask() {
    // задаём левый верхний край невидимого окна
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    // задаём правый нижний край невидимого окна
    ImGui::SetNextWindowSize(ImVec2(WINDOW_SIZE_X, WINDOW_SIZE_Y));
    // создаём невидимое окно
    ImGui::Begin("Overlay", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);
    // получаем список примитивов, которые будут нарисованы
    auto pDrawList = ImGui::GetWindowDrawList();

    // перебираем окружности из динамического массива окружностей
    for (auto circle: circles) {
        ImColor clr;
        clr = ImColor(200, 100, 150);

        // добавляем в список рисования круг
        pDrawList->AddCircle(
                sf::Vector2i(circle.pos.x, circle.pos.y),
                pow(pow(circle.pos2.x-circle.pos.x,2)+pow(circle.pos2.y-circle.pos.y,2),0.5),
                clr,
                100
        );
    }
    // перебираем углы из динамического массива углоы
    for (auto angle: angles) {
        ImColor clr;
        clr = ImColor(10, 100, 150);

        // рисуем угол
        // вершина угла
        sf::Vector2i pointA = angle.pos1;
        // опорные точки
        sf::Vector2i pointB = angle.pos2;
        sf::Vector2i pointC = angle.pos3;

        // определяем вектор смещения
        sf::Vector2i AB = subtract(pointB, pointA);
        sf::Vector2i AC = subtract(pointC, pointA);


        // получаем максимальную длину отрезка на экране, как длину диагонали экрана
        float maxDistance = std::sqrt(WINDOW_SIZE_X * WINDOW_SIZE_X + WINDOW_SIZE_Y * WINDOW_SIZE_Y);

        // получаем новые точки для рисования, которые гарантируют, что линия
        // будет нарисована до границ экрана
        sf::Vector2i renderPointB = sum(pointA, toInt(mul(toFloat(AB), maxDistance)));
        sf::Vector2i renderPointC = sum(pointA, toInt(mul(toFloat(AC), maxDistance)));


        // рисуем линию
        pDrawList->AddLine(
                pointA,
                renderPointB,
                clr,
                0.5f
        );


        // рисуем линию
        pDrawList->AddLine(
                pointA,
                renderPointC,
                clr,
                0.5f
        );

        // рисуем точку
        pDrawList->AddCircleFilled(
                pointA,
                3,
                clr
        );
        // рисуем точку
        pDrawList->AddCircleFilled(
                pointB,
                3,
                clr
        );
        // рисуем точку
        pDrawList->AddCircleFilled(
                pointC,
                3,
                clr
        );
    }
    // заканчиваем рисование окна
    ImGui::End();
}

// ручное добавление элементов
void ShowAddElem() {
    // если не раскрыта панель `Add Elem`
    if (!ImGui::CollapsingHeader("Add Elem"))
        // заканчиваем выполнение
        return;


    // Инструмент выбора цвета
    if (ImGui::DragInt2("Coords", fAddPosBufL, 0.5f, 0, std::min(WINDOW_SIZE_X, WINDOW_SIZE_Y))) {
        // никаких действий не требуется, достаточно
        // тех изменений буфера, которые imGui выполняет
        // автоматически
    }

    // фиксируем id равный 0 для первого элемента
    ImGui::PushID(0);
    // если нажата кнопка `Set 1`
    if (ImGui::Button("Set 1") and sAddPosBufL[0]!=-1) {
        // добавляем в список окружность
        circles.emplace_back(Circle(sf::Vector2i(fAddPosBufL[0], fAddPosBufL[1]), sf::Vector2i(sAddPosBufL[0], sAddPosBufL[1])));
        fAddPosBufL[0]=-1;
        sAddPosBufL[0]=-1;
    }
    // восстанавливаем буфер id
    ImGui::PopID();

    // говорим imGui, что следующий элемент нужно рисовать на той же линии
    ImGui::SameLine();
    // задаём id, равный одному
    ImGui::PushID(1);
    // если нажата кнопка `Set 2`
    if (ImGui::Button("Set 2"),tAddPosBufR[0]!=-1) {
        // добавляем в список угол
        angles.emplace_back(Angle(sf::Vector2i(fAddPosBufR[0], fAddPosBufR[1]), sf::Vector2i(sAddPosBufR[0], sAddPosBufR[1]),sf::Vector2i(tAddPosBufR[0], tAddPosBufR[1])));
        tAddPosBufR[0]=-1;
        fAddPosBufR[0]=-1;
        sAddPosBufR[0]=-1;
    }
    // восстанавливаем буфер id
    ImGui::PopID();
}



// работа с файлами
void ShowFiles() {
    // если не раскрыта панель `Files`
    if (!ImGui::CollapsingHeader("Files"))
        // заканчиваем выполнение
        return;

    // первый элемент в линии
    ImGui::PushID(0);
    // создаём кнопку загрузки
    if (ImGui::Button("Load")) {
        // загружаем данные из файла
        loadFromFile();
    }
    // восстанавливаем буфер id
    ImGui::PopID();

    // следующий элемент будет на той же строчке
    ImGui::SameLine();
    // второй элемент
    ImGui::PushID(1);
    // создаём кнопку сохранения
    if (ImGui::Button("Save")) {
        // сохраняем задачу в файл
        saveToFile();
    }
    // восстанавливаем буфер id
    ImGui::PopID();

}

// решение задачи
void ShowSolve() {
    // если не раскрыта панель `Solve`
    if (!ImGui::CollapsingHeader("Solve"))
        return;
    // первый элемент в линии
    ImGui::PushID(0);
    // создаём кнопку решения
    if (ImGui::Button("Solve")) {
        solve();
    }

    // восстанавливаем буфер id
    ImGui::PopID();

    // следующий элемент будет на той же строчке
    ImGui::SameLine();
    // второй элемент
    ImGui::PushID(1);

    // создаём кнопку очистки
    if (ImGui::Button("Clear")) {
        // удаляем всё
        circles.clear();
        angles.clear();
    }
    // восстанавливаем буфер id
    ImGui::PopID();
}

// помощь
void ShowHelp() {
    if (!ImGui::CollapsingHeader("Help"))
        return;

    // первый заголовок
    ImGui::Text("ABOUT THIS DEMO:");
    // первый элемент списка
    ImGui::BulletText("Author Yugov Michail 10-1");
    // второй элемент списка
    ImGui::BulletText("Powered by SFML+ImGui");
    // разделитель
    ImGui::Separator();

    // второй заголовок
    ImGui::Text("TASK:");
    // первый элемент списка(многострочный)
    ImGui::BulletText("A set of circles and a set of angles are given on the plane\n"
                      "This program is made to find an angle and a circle\n"
                      "for which the area within their intersection achieves the maximum value\n"
                      "");
    // разделитель
    ImGui::Separator();

}


// главный метод
int main() {
    // создаём окно для рисования
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE_X, WINDOW_SIZE_Y), "Geometry Project 10");
    // задаём частоту перерисовки окна
    window.setFramerateLimit(60);
    // инициализация imgui+sfml
    ImGui::SFML::Init(window);

    // задаём цвет фона
    setColor(color);

    // переменная таймера
    sf::Clock deltaClock;
    // пока окно открыто, запускаем бесконечный цикл
    while (window.isOpen()) {
        // создаём событие sfml
        sf::Event event;
        // пока окно принимает события
        while (window.pollEvent(event)) {
            // отправляем события на обработку sfml
            ImGui::SFML::ProcessEvent(event);

            // если событие - это закрытие окна
            if (event.type == sf::Event::Closed) {
                // закрываем окно
                window.close();
            }
            // если событие - это клик мышью
            if (event.type == sf::Event::MouseButtonPressed) {
                // если мышь не обрабатывается элементами imGui
                if (!ImGui::GetIO().WantCaptureMouse) {
                    // если левая кнопка мыши
                    if (event.mouseButton.button == sf::Mouse::Button::Left) {
                        if (fAddPosBufL[0]==-1) {
                            fAddPosBufL[0] = event.mouseButton.x;
                            fAddPosBufL[1] = event.mouseButton.y;
                        } else if (sAddPosBufL[0]==-1) {
                            sAddPosBufL[0] = event.mouseButton.x;
                            sAddPosBufL[1] = event.mouseButton.y;
                        }
                        if (sAddPosBufL[0]!=-1) {
                            circles.emplace_back(sf::Vector2i(fAddPosBufL[0], fAddPosBufL[1]),
                                                 sf::Vector2i(sAddPosBufL[0], sAddPosBufL[1]));
                            fAddPosBufL[0] = -1;
                            sAddPosBufL[0] = -1;
                        }
                    }else if (event.mouseButton.button == sf::Mouse::Button::Right) {
                        if (fAddPosBufR[0]==-1) {
                            fAddPosBufR[0] = event.mouseButton.x;
                            fAddPosBufR[1] = event.mouseButton.y;
                        } else if ( sAddPosBufR[0]==-1){
                            sAddPosBufR[0] = event.mouseButton.x;
                            sAddPosBufR[1] = event.mouseButton.y;
                        } else if (tAddPosBufR[0]==-1){
                            tAddPosBufR[0] = event.mouseButton.x;
                            tAddPosBufR[1] = event.mouseButton.y;
                        }
                        if (tAddPosBufR[0]!=-1) {
                            angles.emplace_back(sf::Vector2i(fAddPosBufR[0], fAddPosBufR[1]),
                                                sf::Vector2i(sAddPosBufR[0], sAddPosBufR[1]),
                                                sf::Vector2i(tAddPosBufR[0], tAddPosBufR[1]));
                            fAddPosBufR[0] = -1;
                            sAddPosBufR[0] = -1;
                            tAddPosBufR[0] = -1;
                        }
                    }
                }
            }
        }

        // запускаем обновление окна по таймеру с заданной частотой
        ImGui::SFML::Update(window, deltaClock.restart());

        // рисование задания
        RenderTask();

        // делаем окно полупрозрачным
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.12f, 0.12f, 0.13f, 0.8f));

        // создаём окно управления
        ImGui::Begin("Control");

        // рисование параметров цвета
        ShowBackgroundSetting();
        // ручное добавление элементов
        ShowAddElem();
        // добавление случайных точек

        // работа с файлами
        ShowFiles();
        // решение задачи
        ShowSolve();
        // помощь
        ShowHelp();

        // конец рисования окна
        ImGui::End();

        // Возвращаем цвет окна к исходному
        ImGui::PopStyleColor();

        // очищаем окно
        window.clear(bgColor);
        // рисуем по окну средствами imgui+sfml
        ImGui::SFML::Render(window);
        // отображаем изменения на окне
        window.display();
    }

    // завершаем работу imgui+sfml
    ImGui::SFML::Shutdown();

    return 0;
}