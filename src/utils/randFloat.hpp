float randF() {
    return (float)rand() / (float)RAND_MAX;
}

float randF(float a, float b) {
    return a + (b - a) * ((float)rand() / (float)RAND_MAX);
}