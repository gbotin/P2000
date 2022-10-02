void door_setup(Position pos);
long door_getPosition();
void door_set(Direction direction, void (*save)(long pos, Direction dir));
void door_move(Direction direction, bool (*cond)(), void (*after)());
void door_toggle(Direction direction);
