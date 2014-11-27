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
    
    float4 position, velocity;
    float4 accel;
    float m;
    
    float4 pos, vel;
    float4 vec_dr;
    float r;
    
    const float G = 6.6354E4f;/*6.673*/
    
    /*get_global_id(2) * get_global_size(1) * get_global_size(0) + */
    id = get_global_id(1) * get_global_size(0) + get_global_id(0);
    pos_id = id + id + id;
    i = 0;
    
    if(id >= count) return;
    
    position = (float4)(positions_in[pos_id], positions_in[pos_id + 1], positions_in[pos_id + 2], 0.0f);
    velocity = (float4)(velocities_in[pos_id], velocities_in[pos_id + 1], velocities_in[pos_id + 2], 0.0f);
    
    accel = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
    for(i = 0; i < count; i++){
        if(i == id)    continue;
        /*if(i != id)*/    pos_i = i + i + i;
        /*if(i != id)*/    pos = (float4)(positions_in[pos_i], positions_in[pos_i + 1], positions_in[pos_i + 2], 0.0f);
        /*if(i != id)*/    m = masses[i];
        vec_dr = pos - position;
        r = length(vec_dr);
        if(r < 0.00001f) r = 0.00001f;
        vec_dr = normalize(vec_dr);
        accel += vec_dr * m / ( r * r );
    }
    accel *=  G;
    vel = accel * dt;
    position += (velocity + vel) * dt;
    velocity += vel;
    
    velocities_out[pos_id    ] = velocity.x;
    velocities_out[pos_id + 1] = velocity.y;
    velocities_out[pos_id + 2] = velocity.z;
    
    positions_out[pos_id    ] = position.x;
    positions_out[pos_id + 1] = position.y;
    positions_out[pos_id + 2] = position.z;
}
