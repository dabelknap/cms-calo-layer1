#include "VMEStream.h"

#include "minunit.h"

#include <stdio.h>
#include <assert.h>


int tests_run = 0;


static char* test_ram1()
{
    CircularBuffer* pc_input    = cbuffer_new();
    CircularBuffer* pc_output   = cbuffer_new();
    CircularBuffer* orsc_input  = cbuffer_new();
    CircularBuffer* orsc_output = cbuffer_new();

    VMEStream *pc_stream = vmestream_initialize_heap(pc_input, pc_output, 1);

    VMEStream *orsc_stream = malloc(sizeof(VMEStream));
    orsc_stream->input              = orsc_input;
    orsc_stream->output             = orsc_output;
    orsc_stream->local_send_size    = pc_stream->remote_send_size;
    orsc_stream->local_recv_size    = pc_stream->remote_recv_size;
    orsc_stream->remote_send_size   = pc_stream->local_send_size;
    orsc_stream->remote_recv_size   = pc_stream->local_recv_size;
    orsc_stream->recv_data          = pc_stream->send_data;
    orsc_stream->send_data          = pc_stream->recv_data;
    orsc_stream->MAXRAM             = pc_stream->MAXRAM;

    for (uint32_t i = 0; i < 20; ++i) {
        cbuffer_push_back(pc_input, 0xDEADBEEF + i);
        cbuffer_push_back(orsc_input, 0xBEEFCAFE + i);
    }

    // initial transfer
    vmestream_transfer_data(pc_stream);

    // transfer data
    vmestream_transfer_data(orsc_stream);
    vmestream_transfer_data(pc_stream);

    mu_assert("Error: orsc_output.pop != DEADBEEF", cbuffer_pop_front(orsc_output) == 0xDEADBEEF);
    mu_assert("Error: pc_output.pop != BEEFCAFE", cbuffer_pop_front(pc_output) == 0xBEEFCAFE);

    // extra transfer is needed to reset the size registers
    // to zero to prepare for another transfer
    vmestream_transfer_data(orsc_stream);
    vmestream_transfer_data(pc_stream);

    // transfer data
    vmestream_transfer_data(orsc_stream);
    vmestream_transfer_data(pc_stream);

    mu_assert("Error: orsc_output.pop != DEADBEEF + 1", cbuffer_pop_front(orsc_output) == 0xDEADBEEF + 1);
    mu_assert("Error: pc_output.pop != BEEFCAFE + 1", cbuffer_pop_front(pc_output) == 0xBEEFCAFE + 1);

    /*
    printf("pc_output.size: %d\n", cbuffer_size(pc_output));
    printf("orsc_output.size: %d\n", cbuffer_size(orsc_output));
    printf("pc_input.size: %d\n", cbuffer_size(pc_input));
    printf("orsc_input.size: %d\n", cbuffer_size(orsc_input));
    */

    mu_assert("Error: pc_output.size != 0", cbuffer_size(pc_output) == 0);
    mu_assert("Error: orsc_output.size != 0", cbuffer_size(orsc_output) == 0);
    mu_assert("Error: pc_input.size != 18", cbuffer_size(pc_input) == 18);
    mu_assert("Error: orsc_input.size != 18", cbuffer_size(orsc_input) == 18);

    // transfer on pc_stream 2x in a row. Nothing should happen.
    vmestream_transfer_data(pc_stream);
    mu_assert("Error: pc_output.size != 0", cbuffer_size(pc_output) == 0);
    mu_assert("Error: orsc_output.size != 0", cbuffer_size(orsc_output) == 0);
    mu_assert("Error: pc_input.size != 18", cbuffer_size(pc_input) == 18);
    mu_assert("Error: orsc_input.size != 18", cbuffer_size(orsc_input) == 18);

    // reset
    vmestream_transfer_data(orsc_stream);
    vmestream_transfer_data(pc_stream);

    vmestream_transfer_data(orsc_stream);
    mu_assert("Error: pc_output.size != 0", cbuffer_size(pc_output) == 0);
    mu_assert("Error: orsc_output.size != 1", cbuffer_size(orsc_output) == 1);
    mu_assert("Error: pc_input.size != 17", cbuffer_size(pc_input) == 17);
    mu_assert("Error: orsc_input.size != 17", cbuffer_size(orsc_input) == 17);

    vmestream_destroy_heap(pc_stream);
    free(orsc_stream);
    cbuffer_free(pc_input);
    cbuffer_free(orsc_input);
    cbuffer_free(pc_output);
    cbuffer_free(orsc_output);

    return 0;
}


static char* all_tests()
{
    mu_run_test(test_ram1);

    return 0;
}
    

/*
static char *test_ram1()
{
    // local application buffers
    CircularBuffer *tx1 = cbuffer_new();
    CircularBuffer *rx1 = cbuffer_new();
    CircularBuffer *tx2 = cbuffer_new();
    CircularBuffer *rx2 = cbuffer_new();

    VMEStream *test1 = vmestream_initialize(tx1, rx1, 1);
    VMEStream *test2 = malloc(sizeof(VMEStream));
    test2->input = tx2;
    test2->output = rx2;

    test2->rx_size = test1->tx_size;
    test2->tx_size = test1->rx_size;
    test2->rx_data = test1->tx_data;
    test2->tx_data = test1->rx_data;
    test2->MAXRAM  = test1->MAXRAM;

    for (unsigned int i = 0; i < 20; ++i) {
        // put some output data on host #1
        cbuffer_push_back(tx1, 0xDEADBEEF + i);
        // put some output data on host #2
        cbuffer_push_back(tx2, 0xBEEFCAFE + i);
    }

    // do a transfer
    vmestream_transfer_data(test1); // step 1 
    vmestream_transfer_data(test2); // step 2

    // host #2 has received data, since host #1 filled it's TX buffer in step 1
    // and host #2 can read it out in step 2
    mu_assert("Error: 0xDEADBEEF != rx2.pop", 0xDEADBEEF == cbuffer_pop_front(rx2));

    vmestream_transfer_data(test1); // step 3
    // now host #1 can read the data loaded by host #2 in step 2
    mu_assert("Error: 0xBEEFCAFE != rx1.pop", 0xBEEFCAFE == cbuffer_pop_front(rx1));

    // do another transfer
    vmestream_transfer_data(test2);
    vmestream_transfer_data(test1);

    mu_assert("Error: 0xBEEFCAFE+1 != rx1.pop", 0xBEEFCAFE + 1 == cbuffer_pop_front(rx1));
    mu_assert("Error: 0xDEADBEEF+1 != rx2.pop", 0xDEADBEEF + 1 == cbuffer_pop_front(rx2));

    // We have consumed all received data (via pop).  There is a word of 
    // data in limbo for host #1
    mu_assert("Error: 0 != rx1.size", 0 == cbuffer_size(rx1));
    mu_assert("Error: 0 != rx2.size", 0 == cbuffer_size(rx2));
    mu_assert("Error: 17 != tx1.size", 17 == cbuffer_size(tx1));
    mu_assert("Error: 18 != tx2.size", 18 == cbuffer_size(tx2));

    // call transfer on #1 twice in a row.  Since it's still waiting for
    // #2 to read the data, nothing happens.
    vmestream_transfer_data(test1);
    mu_assert("Error: 0 != rx1.size", 0 == cbuffer_size(rx1));
    mu_assert("Error: 0 != rx2.size", 0 == cbuffer_size(rx2));
    mu_assert("Error: 17 != tx1.size", 17 == cbuffer_size(tx1));
    mu_assert("Error: 18 != tx2.size", 18 == cbuffer_size(tx2));

    // #2 receives limbo data, puts one of it's words in limbo.
    vmestream_transfer_data(test2);
    mu_assert("Error: 0 != rx1.size", 0 == cbuffer_size(rx1));
    mu_assert("Error: 1 != rx2.size", 1 == cbuffer_size(rx2));
    mu_assert("Error: 17 != tx1.size", 17 == cbuffer_size(tx1));
    mu_assert("Error: 17 != tx2.size", 17 == cbuffer_size(tx2));


    // free memory
    vmestream_destroy(test1);
    free(test2);
    cbuffer_free(tx1);
    cbuffer_free(rx1);
    cbuffer_free(tx2);
    cbuffer_free(rx2);

    return 0;
}
*/


/**
 * Push less data to the buffers than we have RAM available
 */
/*
static char *test_ram2()
{
    // local application buffers
    CircularBuffer *tx1 = cbuffer_new();
    CircularBuffer *rx1 = cbuffer_new();
    CircularBuffer *tx2 = cbuffer_new();
    CircularBuffer *rx2 = cbuffer_new();

    VMEStream *test1 = vmestream_initialize(tx1, rx1, 2);
    VMEStream *test2 = malloc(sizeof(VMEStream));
    test2->input = tx2;
    test2->output = rx2;

    test2->rx_size = test1->tx_size;
    test2->tx_size = test1->rx_size;
    test2->rx_data = test1->tx_data;
    test2->tx_data = test1->rx_data;
    test2->MAXRAM  = test1->MAXRAM;

    // place only one word on the buffers
    cbuffer_push_back(tx1, 0xDEADBEEF);
    // put some output data on host #2
    cbuffer_push_back(tx2, 0xBEEFCAFE);

    vmestream_transfer_data(test1);
    vmestream_transfer_data(test2);
    vmestream_transfer_data(test1);

    mu_assert("Error: tx1 not empty", 0 == cbuffer_size(tx1));
    mu_assert("Error: tx2 not empty", 0 == cbuffer_size(tx2));

    mu_assert("Error: 0xDEADBEEF != rx2.pop", 0xDEADBEEF == cbuffer_pop_front(rx2));
    mu_assert("Error: 0xBEEFCAFE != rx1.pop", 0xBEEFCAFE == cbuffer_pop_front(rx1));

    mu_assert("Error: rx2 not empty", 0 == cbuffer_size(rx2));
    mu_assert("Error: rx1 not empty", 0 == cbuffer_size(rx1));


    // free memory
    vmestream_destroy(test1);
    free(test2);
    cbuffer_free(tx1);
    cbuffer_free(rx1);
    cbuffer_free(tx2);
    cbuffer_free(rx2);

    return 0;
}
*/


/**
 * Overload buffer test
 */
/*
static char *test_buf()
{
    // local application buffers
    CircularBuffer *tx1 = cbuffer_new();
    CircularBuffer *rx1 = cbuffer_new();
    CircularBuffer *tx2 = cbuffer_new();
    CircularBuffer *rx2 = cbuffer_new();

    VMEStream *test1 = vmestream_initialize(tx1, rx1, 32);
    VMEStream *test2 = malloc(sizeof(VMEStream));
    test2->input = tx2;
    test2->output = rx2;

    test2->rx_size = test1->tx_size;
    test2->tx_size = test1->rx_size;
    test2->rx_data = test1->tx_data;
    test2->tx_data = test1->rx_data;
    test2->MAXRAM  = test1->MAXRAM;


    cbuffer_push_back(rx2, 0xDEADBEEF);
    for (int i = 0; i < 510; i++) {
        cbuffer_push_back(rx2, 0xBEEFCAFE);
    }
    cbuffer_push_back(tx1, 0xBEEFCAFE + 1);
    cbuffer_push_back(tx1, 0xBEEFCAFE + 2);
    cbuffer_push_back(tx1, 0xBEEFCAFE + 3);
    cbuffer_push_back(tx1, 0xBEEFCAFE + 4);

    mu_assert("Error: rx2 should have no space left", 
        cbuffer_freespace(rx2) == 0);
    // sanity check
    mu_assert_eq("Error: output size != 4", cbuffer_size(tx1), 4);

    // do several transfers
    vmestream_transfer_data(test1);
    vmestream_transfer_data(test2);
    vmestream_transfer_data(test1);
    vmestream_transfer_data(test2);

    // no data should have been transferred
    mu_assert_eq("Error: tx_size != 4", *(test1->tx_size), 4);
    mu_assert("Error: rx2.pop != 0xDEADBEEF", 0xDEADBEEF == cbuffer_pop_front(rx2));
    cbuffer_pop_front(rx2);
    cbuffer_pop_front(rx2);

    // popping off rx2 should have freed 3 words, but not enough to transfer all
    // four
    vmestream_transfer_data(test1);
    vmestream_transfer_data(test2);

    mu_assert_eq("Error: tx_size != 4", *(test1->tx_size), 4);
    mu_assert("Errrr: rx2.pop not 0xBEEFCAFE", 0xBEEFCAFE == cbuffer_pop_front(rx2));

    cbuffer_pop_front(rx2);
    // now there is enough room for all the limbo data to be transferred to rx2
    vmestream_transfer_data(test1);
    vmestream_transfer_data(test2);

    mu_assert("Error: tx_size != 0", *(test1->tx_size) == 0);
    mu_assert("Error: tx1 not empty", 0 == cbuffer_size(tx1));
    for (int i = 0; i < 4; ++i) {
      mu_assert_eq("Unexpected data transferred", 
          cbuffer_value_at(rx2, cbuffer_size(rx2) - 4 + i), 0xBEEFCAFE + i + 1);
    }


    // free memory
    vmestream_destroy(test1);
    free(test2);
    cbuffer_free(tx1);
    cbuffer_free(rx1);
    cbuffer_free(tx2);
    cbuffer_free(rx2);

    return 0;
}
*/

/*
static char *all_tests()
{
    mu_run_test(test_ram1);
    mu_run_test(test_ram2);
    mu_run_test(test_buf);
    return 0;
}
*/


int main(int argc, char *argv[])
{
    char *result = all_tests();
    if (result != 0) {
        printf("%s\n", result);
    }
    else {
        printf("Tests run: %d\n", tests_run);
        printf("ALL TESTS PASSED\n");
    }

    return result != 0;
}
