// Copyright (c) 2018, 2019 Marko Mahnič
// License: MPL2. See LICENSE in the root of the project.

#include "vectors.h"

#include <cppargparse/argparse-s.h>

#include <gtest/gtest.h>

using namespace argparse;
using namespace testing;

TEST( OptionFactoryTest, shouldCreateASimpleOption )
{
   int i;
   OptionFactory factory;
   auto option = factory.createOption( i );

   EXPECT_FALSE( option.hasVectorValue() );
}

TEST( OptionFactoryTest, shouldCreateAVectorOption )
{
   std::vector<int> v;
   OptionFactory factory;
   auto option = factory.createOption( v );

   EXPECT_TRUE( option.hasVectorValue() );
}

TEST( OptionFactoryTest, shouldCreateAVoidValueOption )
{
   VoidValue i;
   OptionFactory factory;
   auto option = factory.createOption( i );

   EXPECT_FALSE( option.hasVectorValue() );
}

TEST( OptionFactoryTest, shouldFailToCreateAVectorOfValues )
{
   std::vector<VoidValue> v;
   OptionFactory factory;
   EXPECT_THROW( factory.createOption( v ), UnsupportedTargetType );
}

TEST( OptionFactoryTest, shouldUseTheSameValueForTheSameTarget )
{
   int shared;
   int other;
   OptionFactory factory;
   auto o1 = factory.createOption( shared );
   auto o2 = factory.createOption( other );
   auto o3 = factory.createOption( shared );

   EXPECT_TRUE( o1.getValueId() != o2.getValueId() );
   EXPECT_TRUE( o1.getTargetId() != o2.getTargetId() );

   EXPECT_TRUE( o1.getValueId() == o3.getValueId() );
   EXPECT_TRUE( o1.getTargetId() == o3.getTargetId() );

   EXPECT_TRUE( o2.getValueId() != o3.getValueId() );
   EXPECT_TRUE( o2.getTargetId() != o3.getTargetId() );
}

namespace {
struct TestStructure
{
   int shared = 0;
   TestStructure() = default;
   TestStructure( const TestStructure& ) = default;
   TestStructure( const std::string& v )
   {}
   TestStructure& operator=( const TestStructure& ) = default;
   TestStructure& operator=( const std::string& v )
   {
      return *this;
   }
};
}   // namespace

// The structure and the first member have the same address (which is used as a
// target id), but they must not be set through the same Value.
TEST( ValueTest, shouldDistinguishStrutctureTargetFromMemberTarget )
{
   TestStructure test;
   OptionFactory factory;
   auto o1 = factory.createOption( test );
   auto o2 = factory.createOption( test.shared );

   EXPECT_TRUE( o1.getValueId() != o2.getValueId() );
}

TEST( ValueTest, shouldAssignUniqueValueTypeId )
{
   int i;
   unsigned u;
   double d;
   bool b;
   char c;
   TestStructure t;
   auto v0 = VoidValue();
   auto vi = ConvertedValue( i );
   auto vu = ConvertedValue( u );
   auto vd = ConvertedValue( d );
   auto vb = ConvertedValue( b );
   auto vc = ConvertedValue( c );
   auto vt = ConvertedValue( t );
   std::vector<Value*> all{ &v0, &vi, &vu, &vd, &vb, &vc, &vt };

   for ( auto pv1 : all )
      for ( auto pv2 : all )
         if ( pv1 != pv2 )
            EXPECT_TRUE( pv1->getValueTypeId() != pv2->getValueTypeId() );
}