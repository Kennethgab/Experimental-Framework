#include <integration_tests.h>
#include <NewHope.h>
#include <Crypto.h>

bool TestableNewHope::TestingPrimitive() {
  byte* alice_public;
  alice_public = (byte*)malloc(NEWHOPE_SENDABYTES);
  NewHopePrivateKey alice_private;
  NewHope::keygen(alice_public, alice_private);

  byte* bob_public;
  bob_public = (byte*)malloc(NEWHOPE_SENDABYTES);
  byte shared_secretb[32];
  byte shared_secreta[32];
  NewHope::sharedb(shared_secretb, bob_public, alice_public);

  NewHope::shareda(shared_secreta, alice_private, bob_public);
  bool cond = EqualBuffer(shared_secreta, shared_secretb, 32);
  free(alice_public);
  free(bob_public);
  return EqualBuffer(shared_secreta, shared_secretb, 32);
}

// Timing the  device running as Client in Newhope Scheme
float TestableNewHope::TimingPrimitive(SerialLogWriter* logger, size_t samples,
                                       bool verbose_sampling) {
  long start, end, execution_time;
  float avg;
  long total = 0;
  if (verbose_sampling) {
    PrintSimpleHeader(logger, "newhope_client", samples);
    PrintSimpleColumns(logger);
  }
  byte* publickey_buffer;
  NewHopePrivateKey privkey;
  publickey_buffer = (byte*)malloc(NEWHOPE_SENDABYTES);
  byte shared_secret[32];
  for (size_t i = 0; i < samples; i++) {
    RNG.loop();
    NewHope::keygen(publickey_buffer, privkey);
    start = micros();
    // use shared buffer to save memory
    NewHope::sharedb(shared_secret, publickey_buffer, publickey_buffer);
    end            = micros();
    execution_time = end - start;
    total += execution_time;
    if (verbose_sampling) {
      PrintSimpleRow(logger, i + 1, execution_time);
    }
  }
  avg = total / (float)samples;
  if (!verbose_sampling) {
    PrintSimpleAvg(logger, "newhope_client", avg);
  }
  return avg;
}