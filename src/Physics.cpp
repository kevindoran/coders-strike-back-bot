//
// Created by Kevin on 1/08/2016.
//

#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>

#include "Physics.h"


#include "State.h"


double Physics::turnAngle(const PodState& pod, const Vector& target) {
    // turn angle = min(turn_left_angle, turn_right_angle)
    double target_angle = Physics::angleTo(pod.pos, target);
    // Need to have >= and <= instead of > and < as we want 0 degrees instead of 360 when there is no
    // turning to do.
    double turn_left = pod.angle >= target_angle ? pod.angle - target_angle : 2*M_PI - target_angle + pod.angle;
    double turn_right = pod.angle <= target_angle ? target_angle - pod.angle : 2*M_PI - pod.angle + target_angle;
    if(turn_right < turn_left) {
        return turn_right;
    } else {
        // Left means negative angle.
        return -turn_left;
    }
}

PodState Physics::move(const Race& race, const PodState& pod, double acc, double angle, double time) {
    // Angle
    if(angle < - MAX_ANGLE) angle = -MAX_ANGLE;
    else if(angle > MAX_ANGLE) angle = MAX_ANGLE;
    int nextAngle = pod.angle + angle;
    // Apply thrust
    Vector accVector = Vector::fromMagAngle(acc, nextAngle);
    Vector newSpeed = (pod.vel + accVector) * DRAG;
    Vector pos = pod.pos + newSpeed * time;
    // Passed checkpoint test.
    int nextCheckpoint = pod.nextCheckpoint;
    if(passedCheckpoint(pod.pos, pos, race.checkpoints[pod.nextCheckpoint])) {
        nextCheckpoint = (nextCheckpoint + 1) % race.checkpoints.size();
    }
    // Need rounding somewhere, or maybe just truncating.
    PodState* nextState = new PodState(pos, newSpeed, (int)nextAngle, nextCheckpoint);
    return *nextState;
}

/**
 * Calculate intersetion of travel path and checkpoint radius (line-circle intersection).
 */
bool Physics::passedCheckpoint(const Vector& beforePos, const Vector& afterPos, const Checkpoint& checkpoint) {
    Vector D = afterPos -beforePos;
    Vector F = beforePos - checkpoint.pos;
    // t^2(D*D) + 2t(F*D) + (F*F - r^2) = 0
    long a = D.dotProduct(D);
    long b = 2 * F.dotProduct(D);
    long c = F.dotProduct(F) - CHECKPOINT_WIDTH * CHECKPOINT_WIDTH;
    long discrimiminant = b * b - 4 * a * c;
    if(discrimiminant < 0) {
        return false;
    } else {
        double disc = sqrt(discrimiminant);
        double t1 = (-b - disc)/(2*a);
        double t2 = (-b + disc)/(2*a);
        return (t1 >= 0 && t1 <= 1) || (t2 >= 0 && t2 <= 1);
    }
}
/**
 * Calculates the angle of the vector from one point to another (measured from the positive x-axis clockwise). In other
 * words, calculate the angle the vector (toPoint - fromPoint) makes with the positive x-axis.
 * @param fromPoint
 * @param toPoint
 * @return
 */
double Physics::angleTo(const Vector& fromPoint, const Vector& toPoint) {
    Vector diff = toPoint - fromPoint;
    double angle = acos(diff.x / diff.getLength());
    if(diff.y < 0) {
        angle = 2 * M_PI - angle;
    }
    return angle;
}
/**
 * Calculate the angle between two vectors; the angle in the wedge created by two vectors.
 *
 * @return angle between 0 and 2*pi
 */
double Physics::angleBetween(const Vector& from, const Vector& to) {
    double angle = acos(from.dotProduct(to) / (double) (from.getLength() * to.getLength()));
    if(to.y < from.y) {
        angle = 2 * M_PI - angle;
    }
    return angle;
}

double Physics::radToDegrees(double radians) {
    return 360 * radians / 2 * M_PI;
}
