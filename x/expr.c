struct ExprNode {
  std::atomic<uint64_t> cnt;
  union {
    int64_t i;
    uint64_t u;
    float s;
    double d;
    void* p;
  } dat;
  enum BracketType bra;
  ExprNode** sub;
};

extern "C" {

ExprNode* init(ExprNode& e) {
  e.cnt = 1;
  return &e;
}

ExprNode* copy(ExprNode& e) {
  ++e.cnt;
  return &e;
}

void unuse(ExprNode* e) {
}
