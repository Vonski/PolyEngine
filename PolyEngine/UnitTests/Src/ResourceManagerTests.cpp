#include <catch.hpp>

#include <ResourceManager.hpp>

using namespace Poly;

class DummyResource : public Poly::ResourceBase
{
public:
	DummyResource(const String& /*path*/) {}
};

namespace Poly {
	DECLARE_RESOURCE(DummyResource, gDummyResourcesMap)
}
DEFINE_RESOURCE(DummyResource, gDummyResourcesMap)

TEST_CASE("ResourceManager loading/freeing", "[ResourceManager]")
{
	DummyResource* res1 = ResourceManager<DummyResource>::Load("a");
	DummyResource* res2 = ResourceManager<DummyResource>::Load("a");
	DummyResource* res3 = ResourceManager<DummyResource>::Load("b");
	DummyResource* res4 = ResourceManager<DummyResource>::Load("c");

	REQUIRE(res1 == res2);
	REQUIRE(res1 != res3);
	REQUIRE(res3 != res4);
	REQUIRE(res1->GetRefCount() == 2);
	REQUIRE(res2->GetRefCount() == 2);
	REQUIRE(res3->GetRefCount() == 1);
	REQUIRE(res4->GetRefCount() == 1);

	ResourceManager<DummyResource>::Release(res2);
	res2 = nullptr;
	REQUIRE(res1->GetRefCount() == 1);
	DummyResource* res5 = ResourceManager<DummyResource>::Load("a");
	REQUIRE(res1->GetRefCount() == 2);
	REQUIRE(res1 == res5);
	ResourceManager<DummyResource>::Release(res5);
	res2 = nullptr;
	REQUIRE(res1->GetRefCount() == 1);

	ResourceManager<DummyResource>::Release(res1);
	ResourceManager<DummyResource>::Release(res3);
	ResourceManager<DummyResource>::Release(res4);
}
