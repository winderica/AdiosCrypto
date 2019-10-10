#pragma once

random_device randomDevice;
seed_seq seedSequence = {randomDevice(), randomDevice(), randomDevice(), randomDevice(), randomDevice(), randomDevice(), randomDevice(),
                         randomDevice()};
mt19937 mersenneTwisterEngine(seedSequence);

uniform_int_distribution<> dist(INT32_MIN, INT32_MAX);

auto randMT() {
    return (unsigned) dist(mersenneTwisterEngine);
}
