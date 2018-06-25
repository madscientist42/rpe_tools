class NONCOPY
{
protected:
   NONCOPY()  = default;
   ~NONCOPY() = default;
private:
   NONCOPY(const NONCOPY&)            = delete;
   NONCOPY& operator=(const NONCOPY&) = delete;
   NONCOPY(NONCOPY&&)                 = delete;
   NONCOPY& operator=(NONCOPY&&)      = delete;
};
