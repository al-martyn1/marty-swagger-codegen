#pragma once


struct TestInnerStruct
{
    ~TestInnerStruct() { isAlive = false; }

    std::string strValue = "Hello from TestInnerStruct!";
    bool isAlive = true;
};

struct TestStruct
{
    ~TestStruct() { isAlive = false; }

    bool isAlive = true;
    int64_t intValue{};
    double dblValue{};
    bool boolValue{};
    std::string strValue;
    std::wstring wstrValue;
    std::vector<std::string> strings;
    std::shared_ptr<TestInnerStruct> innerStruct;
    std::vector<std::shared_ptr<TestInnerStruct>> innerStructList;
};


namespace jinja2
{
template<>
struct TypeReflection<TestInnerStruct> : TypeReflected<TestInnerStruct>
{
    using FieldAccessor = typename jinja2::TypeReflected<TestInnerStruct>::FieldAccessor;

    static auto& GetAccessors()
    {
        static std::unordered_map<std::string, FieldAccessor> accessors = {
            { "strValue",
              [](const TestInnerStruct& obj) {
                  assert(obj.isAlive);
                  return obj.strValue;
              } },
        };

        return accessors;
    }
};

template<>
struct TypeReflection<TestStruct> : TypeReflected<TestStruct>
{
    using FieldAccessor = typename jinja2::TypeReflected<TestStruct>::FieldAccessor;

    static auto& GetAccessors()
    {
        static std::unordered_map<std::string, FieldAccessor> accessors = {
            { "intValue",
              [](const TestStruct& obj) {
                  assert(obj.isAlive);
                  return jinja2::Reflect(obj.intValue);
              } },
            { "intEvenValue",
              [](const TestStruct& obj) -> Value {
                  assert(obj.isAlive);
                  if (obj.intValue % 2)
                      return {};
                  return { obj.intValue };
              } },
            { "dblValue",
              [](const TestStruct& obj) {
                  assert(obj.isAlive);
                  return jinja2::Reflect(obj.dblValue);
              } },
            { "boolValue",
              [](const TestStruct& obj) {
                  assert(obj.isAlive);
                  return jinja2::Reflect(obj.boolValue);
              } },
            { "strValue",
              [](const TestStruct& obj) {
                  assert(obj.isAlive);
                  return jinja2::Reflect(obj.strValue);
              } },
            { "wstrValue",
              [](const TestStruct& obj) {
                  assert(obj.isAlive);
                  return jinja2::Reflect(obj.wstrValue);
              } },
            { "strViewValue",
              [](const TestStruct& obj) {
                  assert(obj.isAlive);
                  return jinja2::Reflect(nonstd::string_view(obj.strValue));
              } },
            { "wstrViewValue",
              [](const TestStruct& obj) {
                  assert(obj.isAlive);
                  return jinja2::Reflect(nonstd::wstring_view(obj.wstrValue));
              } },
            { "strings",
              [](const TestStruct& obj) {
                  assert(obj.isAlive);
                  return jinja2::Reflect(obj.strings);
              } },
            { "innerStruct",
              [](const TestStruct& obj) {
                  assert(obj.isAlive);
                  #if 0
                  if (obj.innerStruct)
                      return Value("innerStruct field is valid");
                  else
                      return Value("invalid innerStruct field value");
                  #else
                      return obj.innerStruct ? jinja2::Reflect(obj.innerStruct) : Value();
                  #endif
              } },
            { "innerStructList",
              [](const TestStruct& obj) {
                  assert(obj.isAlive);
                  return jinja2::Reflect(obj.innerStructList);
              } },
            { "tmpStructList",
              [](const TestStruct& obj) {
                  assert(obj.isAlive);
                  using list_t = std::vector<std::shared_ptr<TestInnerStruct>>;
                  list_t vals;
                  for (int n = 0; n < 10; ++n)
                      vals.push_back(std::make_shared<TestInnerStruct>());
                  return jinja2::Reflect(list_t(vals.begin(), vals.end()));
              } },
            { "basicCallable",
              [](const TestStruct& obj) {
                  return jinja2::MakeCallable([&obj]() {
                      assert(obj.isAlive);
                      return obj.intValue;
                  });
              } },
            { "getInnerStruct",
              [](const TestStruct& obj) {
                  return jinja2::MakeCallable([&obj]() {
                      assert(obj.isAlive);
                      return jinja2::Reflect(obj.innerStruct);
                  });
              } },
            { "getInnerStructValue",
              [](const TestStruct& obj) {
                  return jinja2::MakeCallable([&obj]() {
                      assert(obj.isAlive);
                      return jinja2::Reflect(*obj.innerStruct);
                  });
              } },
        };

        return accessors;
    }
};
} // namespace jinja2


