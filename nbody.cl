__kernel void kernel_main(unsigned int count,
                           __global float* positions_in, __global float* positions_out,
                           __global float* velocities_in, __global float* velocities_out,
                           __global float* masses,
                           float dt)
{
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

    id = get_global_id(1) * get_global_size(0) + get_global_id(0);
    pos_id = id * 3;
    
    if(id >= count) return;
    
    position = (float3)(positions_in[pos_id], positions_in[pos_id + 1], positions_in[pos_id + 2]);
    velocity = (float3)(velocities_in[pos_id], velocities_in[pos_id + 1], velocities_in[pos_id + 2]);
    
    accel = (float3)(0.0f, 0.0f, 0.0f);

    for(i = 0; i < count; i++){
        if(i == id)    continue;
        pos_i = i * 3;
        pos = (float3)(positions_in[pos_i], positions_in[pos_i + 1], positions_in[pos_i + 2]);
        m = masses[i];
        vec_dr = pos - position;
        r = length(vec_dr);
        if(r < 0.00001f) r = 0.00001f;
        vec_dr = vec_dr / r;
        accel += vec_dr * m / ( r * r );
    }

    accel *=  G;
    velocity += accel * dt;
    position += velocity * dt;
    
    velocities_out[pos_id    ] = velocity.x;
    velocities_out[pos_id + 1] = velocity.y;
    velocities_out[pos_id + 2] = velocity.z;
    
    positions_out[pos_id    ] = position.x;
    positions_out[pos_id + 1] = position.y;
    positions_out[pos_id + 2] = position.z;
}
