#version 430 core

layout(local_size_x = 20, local_size_y = 20) in;

layout(std430, binding = 0) buffer PosIn {
	vec4 PositionIn[];
};
layout(std430, binding = 1) buffer PosOut {
	vec4 PositionOut[];
};
layout(std430, binding = 2) buffer VelIn {
	vec4 VelocityIn[];
};
layout(std430, binding = 3) buffer VelOut {
	vec4 VelocityOut[];
};

uniform float time;

void main() {

	uvec3 nParticles = gl_NumWorkGroups * gl_WorkGroupSize;
	uint idx = gl_GlobalInvocationID.y * nParticles.x +
		gl_GlobalInvocationID.x;

	float ParticleMass = 0.03;
	float ParticleInvMass = 1.0 / 0.03;
	float SpringK = 400.0;
	float RestLengthHoriz = 0.7/2;
	float RestLengthVert = 0.5/2;
	float RestLengthDiag = 0.86/2;
	float DeltaT = 0.00005;
	float DampingConst = 0.2;
	

	vec3 p = vec3(PositionIn[idx]);
	vec3 v = vec3(VelocityIn[idx]), r;
		
	vec3 force = vec3(sin(time)*5,0,1.0);

	if (gl_GlobalInvocationID.y < nParticles.y - 1) {
		r = PositionIn[idx + nParticles.x].xyz - p;
		force += normalize(r)*SpringK*(length(r) -
			RestLengthVert);
	}

	if (gl_GlobalInvocationID.y > 0) {
		r = PositionIn[idx - nParticles.x].xyz - p;
		force += normalize(r)*SpringK*(length(r) -
			RestLengthVert);
	}

	if (gl_GlobalInvocationID.x < nParticles.x - 1) {
		r = PositionIn[idx + 1].xyz - p;
		force += normalize(r)*SpringK*(length(r) -
			RestLengthHoriz);
	}

	if (gl_GlobalInvocationID.x > 0) {
		r = PositionIn[idx - 1].xyz - p;
		force += normalize(r)*SpringK*(length(r) -
			RestLengthHoriz);
	}

	if (gl_GlobalInvocationID.x > 0 &&
		gl_GlobalInvocationID.y < nParticles.y - 1) {
		r = PositionIn[idx + nParticles.x - 1].xyz - p;
		force += normalize(r)*SpringK*(length(r) -
			RestLengthDiag);
	}

	if (gl_GlobalInvocationID.x < nParticles.x - 1 &&
		gl_GlobalInvocationID.y < nParticles.y - 1) {
		r = PositionIn[idx + nParticles.x + 1].xyz - p;
		force += normalize(r)*SpringK*(length(r) -
			RestLengthDiag);
	}

	if (gl_GlobalInvocationID.x > 0 &&
		gl_GlobalInvocationID.y > 0) {
		r = PositionIn[idx - nParticles.x - 1].xyz - p;
		force += normalize(r)*SpringK*(length(r) -
			RestLengthDiag);
	}

	if (gl_GlobalInvocationID.x < nParticles.x - 1 &&
		gl_GlobalInvocationID.y > 0) {
		r = PositionIn[idx - nParticles.x + 1].xyz - p;
		force += normalize(r)*SpringK*(length(r) -
			RestLengthDiag);
	}

	force -= DampingConst * v;

	vec3 a = force * ParticleInvMass;
	PositionOut[idx] = vec4(p + v * DeltaT + 0.5 * a * DeltaT * DeltaT, 1.0);
	VelocityOut[idx] = vec4(v + a * DeltaT, 0.0);
		
	if (gl_GlobalInvocationID.x == 0) {
		PositionOut[idx] = vec4(p, 1.0);
		VelocityOut[idx] = vec4(0, 0, 0, 0);
	}
		
}