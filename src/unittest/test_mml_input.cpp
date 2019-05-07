#include <cppunit/extensions/HelperMacros.h>
#include "../mml_input.h"
#include "../song.h"
#include "../track.h"

class MML_Input_Test : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(MML_Input_Test);
	CPPUNIT_TEST(test_basic_mml);
	CPPUNIT_TEST(test_mml_track_id);
	CPPUNIT_TEST(test_mml_note_octave);
	CPPUNIT_TEST(test_mml_note_flat_sharp);
	CPPUNIT_TEST(test_mml_note_duration);
	CPPUNIT_TEST(test_mml_loop);
	CPPUNIT_TEST(test_mml_tag_replace);
	CPPUNIT_TEST(test_mml_tag_append);
	CPPUNIT_TEST(test_mml_multi_track);
	CPPUNIT_TEST(test_mml_conditional);
	CPPUNIT_TEST(test_mml_platform_command);
	CPPUNIT_TEST_SUITE_END();
private:
	Song *song;
	MML_Input *mml_input;
public:
	void setUp()
	{
		song = new Song();
		mml_input = new MML_Input(song);
	}
	void tearDown()
	{
		delete mml_input;
		delete song;
	}
	void test_basic_mml()
	{
		mml_input->read_line("A cdef");
		CPPUNIT_ASSERT(song->get_track(0).get_event_count() == 4);
	}
	void test_mml_track_id()
	{
		mml_input->read_line("A cdef");
		mml_input->read_line("B cd");
		mml_input->read_line("*10 cdefga");
		CPPUNIT_ASSERT(song->get_track(0).get_event_count() == 4);
		CPPUNIT_ASSERT(song->get_track(1).get_event_count() == 2);
		CPPUNIT_ASSERT(song->get_track(10).get_event_count() == 6);
	}
	void test_mml_note_flat_sharp()
	{
		mml_input->read_line("A o4c+d-");
		CPPUNIT_ASSERT_EQUAL(Event::NOTE, song->get_track(0).get_event(0).type);
		CPPUNIT_ASSERT_EQUAL((int16_t)37, song->get_track(0).get_event(0).param);
		CPPUNIT_ASSERT_EQUAL(Event::NOTE, song->get_track(0).get_event(1).type);
		CPPUNIT_ASSERT_EQUAL((int16_t)37, song->get_track(0).get_event(1).param);
	}
	void test_mml_note_octave()
	{
		mml_input->read_line("A o4cd>e<f");
		CPPUNIT_ASSERT_EQUAL(Event::NOTE, song->get_track(0).get_event(0).type);
		CPPUNIT_ASSERT_EQUAL((int16_t)36, song->get_track(0).get_event(0).param);
		CPPUNIT_ASSERT_EQUAL(Event::NOTE, song->get_track(0).get_event(1).type);
		CPPUNIT_ASSERT_EQUAL((int16_t)38, song->get_track(0).get_event(1).param);
		CPPUNIT_ASSERT_EQUAL(Event::NOTE, song->get_track(0).get_event(2).type);
		CPPUNIT_ASSERT_EQUAL((int16_t)52, song->get_track(0).get_event(2).param);
		CPPUNIT_ASSERT_EQUAL(Event::NOTE, song->get_track(0).get_event(3).type);
		CPPUNIT_ASSERT_EQUAL((int16_t)41, song->get_track(0).get_event(3).param);
	}
	void test_mml_note_duration()
	{
		mml_input->read_line("A l16cd8e.f8.");
		CPPUNIT_ASSERT_EQUAL((uint16_t)6, song->get_track(0).get_event(0).on_time);
		CPPUNIT_ASSERT_EQUAL((uint16_t)12, song->get_track(0).get_event(1).on_time);
		CPPUNIT_ASSERT_EQUAL((uint16_t)9, song->get_track(0).get_event(2).on_time);
		CPPUNIT_ASSERT_EQUAL((uint16_t)18, song->get_track(0).get_event(3).on_time);
	}
	void test_mml_loop()
	{
		mml_input->read_line("A [c] [c]5");
		CPPUNIT_ASSERT_EQUAL(Event::LOOP_START, song->get_track(0).get_event(0).type);
		CPPUNIT_ASSERT_EQUAL(Event::LOOP_END, song->get_track(0).get_event(2).type);
		CPPUNIT_ASSERT_EQUAL((int16_t)2, song->get_track(0).get_event(2).param); // default parameter
		CPPUNIT_ASSERT_EQUAL(Event::LOOP_START, song->get_track(0).get_event(3).type);
		CPPUNIT_ASSERT_EQUAL(Event::LOOP_END, song->get_track(0).get_event(5).type);
		CPPUNIT_ASSERT_EQUAL((int16_t)5, song->get_track(0).get_event(5).param); // default parameter
	}
	void test_mml_tag_replace()
	{
		mml_input->read_line("#title My song title.");
		CPPUNIT_ASSERT_EQUAL(std::string("My song title."), song->get_tag_front("#title"));
		mml_input->read_line("#title My new song title.");
		CPPUNIT_ASSERT_EQUAL(std::string("My new song title."), song->get_tag_front("#title"));
	}
	void test_mml_tag_append()
	{
		mml_input->read_line("@blah One two, three \"four, five\"");
		mml_input->read_line("\tsixth");
		CPPUNIT_ASSERT_EQUAL(std::string("One"), song->get_tag("@blah").at(0));
		CPPUNIT_ASSERT_EQUAL(std::string("two"), song->get_tag("@blah").at(1));
		CPPUNIT_ASSERT_EQUAL(std::string("three"), song->get_tag("@blah").at(2));
		CPPUNIT_ASSERT_EQUAL(std::string("four, five"), song->get_tag("@blah").at(3));
		CPPUNIT_ASSERT_EQUAL(std::string("sixth"), song->get_tag("@blah").at(4));
	}
	void test_mml_multi_track()
	{
		mml_input->read_line("ABC cdef");
		CPPUNIT_ASSERT(song->get_track(0).get_event_count() == 4);
		CPPUNIT_ASSERT(song->get_track(1).get_event_count() == 4);
		CPPUNIT_ASSERT(song->get_track(2).get_event_count() == 4);
	}
	void test_mml_conditional()
	{
		mml_input->read_line("ABC o4c{d/e/f}g");
		CPPUNIT_ASSERT(song->get_track(0).get_event_count() == 3);
		CPPUNIT_ASSERT(song->get_track(1).get_event_count() == 3);
		CPPUNIT_ASSERT(song->get_track(2).get_event_count() == 3);
		for(int i=0; i<3; i++)
		{
			// First and last event should be same in all tracks
			// Also the type of the conditional event (they're all notes)
			CPPUNIT_ASSERT_EQUAL(Event::NOTE, song->get_track(i).get_event(0).type);
			CPPUNIT_ASSERT_EQUAL((int16_t)36, song->get_track(i).get_event(0).param);
			CPPUNIT_ASSERT_EQUAL(Event::NOTE, song->get_track(i).get_event(1).type);
			CPPUNIT_ASSERT_EQUAL(Event::NOTE, song->get_track(i).get_event(2).type);
			CPPUNIT_ASSERT_EQUAL((int16_t)43, song->get_track(i).get_event(2).param);
		}
		// Param should differ
		CPPUNIT_ASSERT_EQUAL((int16_t)38, song->get_track(0).get_event(1).param);
		CPPUNIT_ASSERT_EQUAL((int16_t)40, song->get_track(1).get_event(1).param);
		CPPUNIT_ASSERT_EQUAL((int16_t)41, song->get_track(2).get_event(1).param);
	}
	void test_mml_platform_command()
	{
		mml_input->read_line("A 'first second third'c 'foo bar baz'");
		CPPUNIT_ASSERT(song->get_track(0).get_event_count() == 3);
		CPPUNIT_ASSERT_EQUAL(Event::PLATFORM, song->get_track(0).get_event(0).type);
		CPPUNIT_ASSERT_EQUAL((int16_t)-32768, song->get_track(0).get_event(0).param);
		CPPUNIT_ASSERT_EQUAL(Event::PLATFORM, song->get_track(0).get_event(2).type);
		CPPUNIT_ASSERT_EQUAL((int16_t)-32767, song->get_track(0).get_event(2).param);
		// Check tags
		CPPUNIT_ASSERT_EQUAL(std::string("first"), song->get_platform_command(-32768).at(0));
		CPPUNIT_ASSERT_EQUAL(std::string("second"), song->get_platform_command(-32768).at(1));
		CPPUNIT_ASSERT_EQUAL(std::string("third"), song->get_platform_command(-32768).at(2));
		CPPUNIT_ASSERT_EQUAL(std::string("foo"), song->get_platform_command(-32767).at(0));
		CPPUNIT_ASSERT_EQUAL(std::string("bar"), song->get_platform_command(-32767).at(1));
		CPPUNIT_ASSERT_EQUAL(std::string("baz"), song->get_platform_command(-32767).at(2));
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(MML_Input_Test);

