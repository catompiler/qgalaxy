//#pragma OPENCL EXTENSION cl_khr_fp64 : enable
//#pragma OPENCL EXTENSION cl_amd_printf : enable
//#pragma FP_CONTRACT on
//__attribute__((vec_type_hint(float3)))

#define RADIUS_EPSILON 1e-18f


/**
 * @brief Ядро программы OpenCL.
 * @param count Число тел.
 * @param positions_in Исходные данные - буфер позиций.
 * @param positions_out Результат - буфер позиций.
 * @param velocities_in Исходные данные - буфер скоростей.
 * @param velocities_out Результат - буфер скоростей.
 * @param masses Исходные данные - буфер масс.
 * @param dt Время шага.
 */
__kernel void kernel_main(const unsigned int count,
                           const __global float* positions_in, __global float* positions_out,
                           const __global float* velocities_in, __global float* velocities_out,
                           const __global float* masses, const float dt,
                           __local float* cached_pos, __local float* cached_mass, unsigned int cache_size)
{
    // Локальные переменные. Память: private.
    unsigned int gid;
    //unsigned int lid;
    unsigned int i;
    unsigned int pos_i;
    
    float3 position, velocity;
    float3 accel;
    float m;
    
    float3 pos;
    float3 vec_dr;
    float r;

    /*__local float cached_pos[1024 * 3];
    __local float cached_mass[1024];
    unsigned int cache_size = 1024;*/

    // Итератор по кэшу.
    unsigned int j;
    // Индекс work-item'а в кэше.
    unsigned int cache_index;
    // Число звёзд в кэше.
    unsigned int cache_count;
    // Использованный размер кэша.
    // Если вдруг размер кэша больше чем нужно.
    unsigned int cache_size_used;
    
    /*
    G, LY^3 / (Msun * Year^2)
    */
    const float G = 1.57e-13f;

    // Номер звезды.
    gid = get_global_id(1) * get_global_size(0) + get_global_id(0);
    //lid = get_local_id(1) * get_local_size(0) + get_local_id(0);
    
    // Если этот рабочий элемент - за пределами массива - прекратить работу.
    // Почему-то в реализации с кэшем
    // Это некорректно работает.
    //if(gid >= count) return;

    // Если work-item - звзеда.
    if(gid < count){
        // Загрузим данные.
        // Позиция звезды.
        position = vload3(gid, positions_in);
        // Скорость звезды.
        velocity = vload3(gid, velocities_in);
    }
    
    // Обнулить ускорение.
    accel = (float3)(0.0f, 0.0f, 0.0f);

    // Используемый кэш.
    cache_size_used = min((unsigned int)(get_local_size(0) * get_local_size(1)), cache_size);

    // Индекс в кэше.
    cache_index = gid % cache_size_used;

    for(i = 0; i < count; i += cache_count){
        // Количество данных для загрузки в кэш.
        cache_count = min(cache_size_used, count - i);
        // Смещение в массивах.
        pos_i = i + cache_index;
        // Если не выходим за границы.
        if(cache_index < cache_count){
            // Закэшируем данные.
            // Позиция очередной звезды.
            vstore3(vload3(pos_i, positions_in), cache_index, cached_pos);
            // Масса очередной звезды.
            cached_mass[cache_index] = masses[pos_i];
        }
        // Подождём всех.
        barrier(CLK_LOCAL_MEM_FENCE);

        // Если work-item - звзеда.
        if(gid < count){
            // Посчитаем взаимодействие со звёздами в кэше.
            for(j = 0; j < cache_count; j ++){
                // Не будем взаимодейтсвовать с собой.
                if(i + j == gid) continue;
                // Получим позицию звезды.
                pos = vload3(j, cached_pos);
                // Получим массу звезды.
                m = cached_mass[j];
                // Получим направление вектора скорости и ускорения.
                vec_dr = pos - position;
                // Получим длину вектора.
                r = length(vec_dr);
                // Предотвратим слишком близкое сближение и уход ускорения в бесконечность.
                r = max(r, RADIUS_EPSILON);
                // Нормализуем вектор.
                vec_dr = vec_dr / r;
                // Вычислим и накомпи ускорение от взаимодействия с очередной звездой.
                accel += vec_dr * m / ( r * r );
            }
        }
        // Подождём всех.
        barrier(CLK_LOCAL_MEM_FENCE);
    }
    // Умножим на вынесенную за скобки
    // гравитационную постоянную.
    accel *=  G;

    // Если work-item - звзеда.
    if(gid < count){
        // Вычислим новую скорость.
        velocity += accel * dt;
        // Вычислим новую позицию.
        position += velocity * dt;

        //velocity = (float3)((float)gid, (float)lid, (float)i);

        // Сохраним новую скорость в массив.
        vstore3(velocity, gid, velocities_out);

        // Сохраним новую позицию в массив.
        vstore3(position, gid, positions_out);
    }
}
