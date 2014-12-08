
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
__kernel void kernel_main(unsigned int count,
                           __global float* positions_in, __global float* positions_out,
                           __global float* velocities_in, __global float* velocities_out,
                           __global float* masses,
                           float dt)
{
    // Локальные переменные. Память: private.
    unsigned int id;
    unsigned int pos_id;
    unsigned int i;
    unsigned int pos_i;
    
    float3 position, velocity;
    float3 accel;
    float m;
    
    float3 pos;
    float3 vec_dr;
    float r;
    
    /*
    G, LY^3 / (Msun * Year^2)
    */
    const float G = 1.57e-13;

    // Номер звезды.
    id = get_global_id(1) * get_global_size(0) + get_global_id(0);
    
    // Если этот рабочий элемент - за пределами массива - прекратить работу.
    if(id >= count) return;

    // Смещение позиции и вектора скорости в массивах.
    pos_id = id * 3;
    
    // Получить позицию и скорость из массивов.
    position = (float3)(positions_in[pos_id], positions_in[pos_id + 1], positions_in[pos_id + 2]);
    velocity = (float3)(velocities_in[pos_id], velocities_in[pos_id + 1], velocities_in[pos_id + 2]);
    
    // Обнулить ускорение.
    accel = (float3)(0.0f, 0.0f, 0.0f);

    // Цикл по всем звёздам.
    for(i = 0; i < count; i++){
        // Не будем взаимодейтсвовать с собой.
        if(i == id) continue;
        // Смещение в массивах.
        pos_i = i * 3;
        // Получим позицию звезды.
        pos = (float3)(positions_in[pos_i], positions_in[pos_i + 1], positions_in[pos_i + 2]);
        // Получим массу звезды.
        m = masses[i];
        // Получим направление вектора скорости и ускорения.
        vec_dr = pos - position;
        // Получим длину вектора.
        r = length(vec_dr);
        // Предотвратим слишком близкое сближение и уход ускорения в бесконечность.
        if(r < 0.00001f) r = 0.00001f;
        // Нормализуем вектор.
        vec_dr = vec_dr / r;
        // Вычислим и накомпи ускорение от взаимодействия с очередной звездой.
        accel += vec_dr * m / ( r * r );
    }

    // Умножим на вынесенную за скобки
    // гравитационную постоянную.
    accel *=  G;
    // Вычислим новую скорость.
    velocity += accel * dt;
    // Вычислим новую позицию.
    position += velocity * dt;
    
    // Созраним новую скорость в массив.
    velocities_out[pos_id    ] = velocity.x;
    velocities_out[pos_id + 1] = velocity.y;
    velocities_out[pos_id + 2] = velocity.z;

    // Созраним новую позицию в массив.
    positions_out[pos_id    ] = position.x;
    positions_out[pos_id + 1] = position.y;
    positions_out[pos_id + 2] = position.z;
}
