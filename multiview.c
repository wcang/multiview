#include <gst/gst.h>
#include <stdio.h>

static void cb_newpad(GstElement * decodebin,
    GstPad * pad, gpointer data)
{
  GstCaps * caps;
  GstStructure * str;
  GstPad * videopad;
  GstElement * convert = (GstElement * ) data;
  printf("callback\n");
  /* getting video sink from videoconvert */
  videopad = gst_element_get_static_pad(convert, "sink");

  /* check media type to ensure that it is video */
  caps = gst_pad_query_caps(pad, NULL);
  str = gst_caps_get_structure(caps, 0);

  printf("capability: %s\n", gst_structure_get_name(str));
  if (!g_strrstr(gst_structure_get_name(str), "video")) {
    goto exit_cb;
  }
  gst_pad_link(pad, videopad);
  printf("linking pad\n");
exit_cb:  
  gst_caps_unref(caps);
  gst_object_unref(videopad);
  return;
}


int main(int argc, char * argv[])
{
  GMainLoop * loop;
  GstElement * bin, * pipeline, * source, * sink, * decode, * convert;
  /* init */
  gst_init(&argc, &argv);
  
  /* create */
  pipeline = gst_pipeline_new("pipeline");
  bin = gst_bin_new("bin");
  source = gst_element_factory_make("filesrc", "source");
  decode = gst_element_factory_make("decodebin", "decode");
  convert = gst_element_factory_make("videoconvert", "convert");
  sink = gst_element_factory_make("xvimagesink", "sink");

  /* register callback for pad-added signal from decodebin */
  g_signal_connect(decode, "pad-added", G_CALLBACK(cb_newpad), convert);

  /* add elements to the bin */
  gst_bin_add_many(GST_BIN(bin), source, decode, convert, sink, NULL);

  /* add bin to the pipeline */
  gst_bin_add(GST_BIN(pipeline), bin);

  /* link the elements */
  gst_element_link(source, decode);
  gst_element_link(convert, sink); 

  /* set the source video file */
  printf("first argument: %s\n", argv[1]);
  g_object_set(GST_OBJECT(source), "location", argv[1], NULL);

  /* start the playback */
  gst_element_set_state(pipeline, GST_STATE_PLAYING);

  loop = g_main_loop_new(NULL, FALSE);
  g_main_loop_run(loop);
  gst_object_unref(GST_OBJECT(pipeline));

  return 0;
}
