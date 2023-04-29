#include <vector>
#include <thread>
#include <iostream>
#include <chrono>
#include <mutex>
#include <condition_variable>

// ЧТОБЫ УДОБНО КОМПИЛИРОВАТЬ КОД, РЕАЛИЗУЙТЕ КАЖДОЕ ЗАДАНИЕ В ВИДЕ ФУНКЦИЙ

// 1. Создайте поток, который выведет текущее время с помощью передачи в поток функции print_time()
// Для получения времени используйте функцию std::chrono::system_clock::now(); (1 балл)

void print_time() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t t_c = std::chrono::system_clock::to_time_t(now);
    std::cout << "Current time (1): " << std::ctime(&t_c);
}

void task1(){
    std::cout << std::endl << "TASK 1" << std::endl;
    std::thread thread(print_time);
    thread.join();
}
// 2. Создайте объект-функтор, делающий то же самое, только время считайте с помощью std::time (1 балл)

class Time{
public:
    void operator()(){
        {
            std::time_t result = std::time(nullptr);
            std::cout << "Current time (2): " << std::asctime(std::localtime(&result));
        }
    }
};

void task2(){
    std::cout << std::endl << "TASK 2" << std::endl;
    Time time_object;
    std::thread thread(time_object);
    thread.join();
};


// 3. Создайте поток, который вернет текущее время по ссылке, используя передачу задачи с помощью лямба-функции
// После завершения потока снова замерьте текущее время и напечатайте пройденное время (разницу в мс). (1 балл)

void task3(){
    std::cout << std::endl << "TASK 3" << std::endl;
    std::chrono::time_point<std::chrono::system_clock> t_begin;
    std::chrono::time_point<std::chrono::system_clock> t_end;

    std::thread thread([&t_begin](){t_begin = std::chrono::system_clock::now();});
    thread.join();

    t_end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t_end - t_begin);
    std::cout << "Duration (3): " << duration.count() * 0.001 << " milliseconds" << std::endl;
}


//4. Создайте столько потоков, сколько доступно на вашем компьютере
// (std::thread::hardware_concurrency())
// Каждый поток должен напечатать свой id. Особое внимание обратите на вывод, чтобы он был согласованным (1 балл)

class PrintID{
    std::mutex m;
    std::condition_variable cv;
    size_t idx = 0;
public:
    void operator()(size_t i){
        //idx = 0;
        std::unique_lock<std::mutex> ul(m);
        cv.wait(ul, [&i, this](){return i == idx;});
        std::cout << "Thread ID:" << std::this_thread::get_id() << std::endl;
        //std::this_thread::sleep_for(std::chrono::milliseconds(500));
        idx += 1;
        cv.notify_all();
    }

    void reset(){
        idx = 0;
    }
};
/* ЗАМЕЧАНИЕ: Для того, чтобы можно было использовать printID дальше необходимо сбрасывать счётчик idx.
 * Если делать это в самом operator(), то необходимо ставить задержку перед notify_all(), поскольку
 * если этого не сделать, то в конце выполнения программы следующий поток может просто не создаться.
 * Либо второй вариант - добавить метод reset. */

void task4(){
    std::cout << std::endl << "TASK 4" << std::endl;
    size_t threads_n = std::thread::hardware_concurrency();
    std::cout << "Number of threads: " << threads_n << std::endl;
    std::vector<std::thread> t_vec;
    PrintID printId;

    for (size_t i = 0; i < threads_n; i++){
        t_vec.emplace_back(std::ref(printId), i);
    }
    for (size_t i = 0; i < threads_n; i++){
        t_vec[i].join();
    }
}


// 5. Напечатайте числа ПОСЛЕДОВАТЕЛЬНО от 0 до 10 так, чтобы четные числа выводил один поток, а нечетные - другой.
// Требуемый вывод: 0 1 2 3 4 5 6 7 8 9 10. Используйте механизм блокировки. (2 балла)


class PrintEvenOdd{
    std::mutex m;
    std::condition_variable cv;
    size_t number = 0;
public:
    void operator()(bool isOdd, size_t max_number){
        std::unique_lock<std::mutex> ul(m);
        while (number < max_number){
            cv.wait(ul, [&](){return number % 2 == static_cast<size_t>(isOdd);});
            std::cout << number << " ";
            number += 1;
            cv.notify_all();
        }
    }

    void reset(){
        number = 0;
    }
};


void task5(){
    std::cout << std::endl << "TASK 5" << std::endl;
    PrintEvenOdd printEvenOdd;
    std::thread evenThread(std::ref(printEvenOdd), false, 10);
    std::thread oddThread(std::ref(printEvenOdd), true, 10);

    oddThread.join();
    evenThread.join();

    std::cout << std::endl;
}

// 6. Создайте две матрицы А и B размером 300х300. Напишите параллельную версию перемножения матриц с записью результата
// в отдельную матрицу С. (3 балла)
// Создайте 6 потоков (не считая родительский).  1 поток должен перемножать строки 0-49 на столбцы 0-49,
// 2 поток - строки 50-99 на столбцы 50-99 и так далее.
// Далее одна из вариаций кода переножения матрицы 3х3:
//int i,j,k;
//for (i=1; i<=3; i++)
//    {
//        for (j=1; j<=3; j++)
//        {
//            sum = 0;
//            for (k=1; k<=3; k++)
//              sum = sum + A[i][k]*B[k][j];
//        }
//    C[i][j] = sum;
//    }
//}



const size_t matrix_size = 300;

//Один из вариантов заполнения матриц
void fill_matrices(int A[matrix_size][matrix_size], int B[matrix_size][matrix_size]){
    for (size_t i = 0; i < matrix_size; i++){
        for (size_t j = 0; j < matrix_size; j++){
            A[i][j] = static_cast<int>(i + 2);
            B[i][j] = static_cast<int>(i) - static_cast<int>(j);
        }
    }
}

void fill_block(int A[matrix_size][matrix_size], int B[matrix_size][matrix_size],
                int res[matrix_size][matrix_size], size_t init_row, size_t end_row){

    int sum = 0;
    for (size_t i = init_row; i < end_row; i++){
        for(size_t j = 0; j < matrix_size; j++){
            sum = 0;
            for (size_t k = 0; k < matrix_size; k++){
                sum += A[i][k] * B[k][j];
            }
            res[i][j] = sum;
        }
    }
}

void task6(){
    std::cout << std::endl << "TASK 6" << std::endl;
    //Заполнение матриц
    int A[matrix_size][matrix_size];
    int B[matrix_size][matrix_size];
    fill_matrices(A, B);

    //Вычисление произведения
    int C[matrix_size][matrix_size];

    std::thread t1(fill_block, A, B, C, 0, 50);
    std::thread t2(fill_block, A, B, C, 50, 100);
    std::thread t3(fill_block, A, B, C, 100, 150);
    std::thread t4(fill_block, A, B, C, 150, 200);
    std::thread t5(fill_block, A, B, C, 200, 250);
    std::thread t6(fill_block, A, B, C, 250, 300);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();

    std::cout << "Some values (indexing starts from 0) " << std::endl;
    std::cout << "C[1][2] = " << C[1][2] << std::endl;
    std::cout << "C[150][200] = " << C[150][200] << std::endl;
    std::cout << "C[299][299] = " << C[299][299] << std::endl;
}

//7. Замерьте время работы параллельного перемножения матриц и последовательного кода.
// Сравните и объясните результаты. (2 балла)


void task7(){
    std::cout << std::endl << "TASK 7" << std::endl;
    std::chrono::time_point<std::chrono::system_clock> t_begin;
    std::chrono::time_point<std::chrono::system_clock> t_end;

    int A[matrix_size][matrix_size];
    int B[matrix_size][matrix_size];

    fill_matrices(A, B);

    int C1[matrix_size][matrix_size]; //Для параллельного умножения
    int C2[matrix_size][matrix_size]; //Для непараллельного умножения

    //Параллельное вычисление
    t_begin = std::chrono::system_clock::now();

    std::thread t1(fill_block, A, B, C1, 0, 50);
    std::thread t2(fill_block, A, B, C1, 50, 100);
    std::thread t3(fill_block, A, B, C1, 100, 150);
    std::thread t4(fill_block, A, B, C1, 150, 200);
    std::thread t5(fill_block, A, B, C1, 200, 250);
    std::thread t6(fill_block, A, B, C1, 250, 300);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();

    t_end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t_end - t_begin);

    std::cout << "Parallel computing duration: " << duration.count() * 0.001 << " milliseconds" << std::endl;
    //Непараллельное вычисление
    t_begin = std::chrono::system_clock::now();

    fill_block(A, B, C2, 0, 300);

    t_end = std::chrono::system_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(t_end - t_begin);

    std::cout << "Non-parallel computing duration: " << duration.count() * 0.001 << " milliseconds" << std::endl;
}
/* КОММЕНТАРИЙ: Параллельное вычисление работает в 3 раза быстрее (15.426 мс против 43.914 мс)
 * (Замечание: на другом компьютере результат может быть другим), что неудивительно, поскольку
 * в случае параллельного нахождения блоков в (почти) каждый момент времени находятся 6 элементов матрицы,
 * в то время как при непараллельном вычислении в каждый момент находится только один элемент матрицы.
 *
 * `ЗАМЕЧАНИЕ: Разница в 3, а не 6 раз обусловлена тем, что матрицы A и B достаточно малы и тем, что потоки создаются
 * последовательно, а не одновременно.*/

int main(){
    task1();
    task2();
    task3();
    task4();
    task5();
    task6();
    task7();
    return 0;
}
