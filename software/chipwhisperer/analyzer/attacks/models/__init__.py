from chipwhisperer.analyzer.attacks.models import AES128_8bit as aes128_leakage
from chipwhisperer.analyzer.attacks.models.AES128_8bit import AES128_8bit
from chipwhisperer.analyzer.attacks.models.AES128_8bit import AES128_ttable
from chipwhisperer.analyzer.attacks.models.AES128_8bit import AES128_ttable_dec
from chipwhisperer.analyzer.attacks.models.AES128_8bit import AES128_prev
import textwrap
from chipwhisperer.analyzer.attacks.models.AES128_8bit import AESLeakageHelper


class EightBitAES128LeakageModels:
    """Contains the available leakage models for 8 bit AES128.

    To use a leakage model during a correlation power analysis attack
    see :func:`cpa <chipwhisperer.analyzer.cpa>`.

    :data:`leakage_models <chipwhisperer.analyzer.leakage_models>` is an instance of
    this class.
    """

    def new_model(self, model):
        """Makes a new leakage model for use with analyzer

        Usage::

            class AES256_Round13_Model(cwa.AESLeakageHelper):
                def leakage(self, pt, ct, guess, bnum):
                    # You must put YOUR recovered 14th round key here.
                    # This example may not be accurate!
                    calc_round_key = [0xea, 0x79, 0x79, 0x20, 0xc8, 0x71,
                                      0x44, 0x7d, 0x46, 0x62, 0x5f, 0x51,
                                      0x85, 0xc1, 0x3b, 0xcb]
                    xored = [calc_round_key[i] ^ pt[i] for i in range(0, 16)]
                    block = xored
                    block = self.inv_shiftrows(block)
                    block = self.inv_subbytes(block)
                    block = self.inv_mixcolumns(block)
                    block = self.inv_shiftrows(block)
                    result = block
                    return self.inv_sbox((result[bnum] ^ guess[bnum]))

            leak_model = cwa.leakage_models.new_model(AES256_Round13_Model)

        Args:
            model (AESLeakageHelper): New leakage model to create

        Returns:
            Leakage model made from model

        """
        return AES128_8bit(model)

    @property
    def t_table(self):
        """Hamming weight of t-table"""
        return AES128_ttable(aes128_leakage.PtKey_XOR)

    @property
    def t_table_dec(self):
        """Hamming weight of inverse t-table"""
        return AES128_ttable_dec(aes128_leakage.PtKey_XOR)

    @property
    def plaintext_key_xor(self):
        """Hamming weight of 1st round key mix (pt ^ key)"""
        return AES128_8bit(aes128_leakage.PtKey_XOR)

    @property
    def sbox_output(self):
        """Hamming weight of 1st round SBox output"""
        return AES128_8bit(aes128_leakage.SBox_output)

    @property
    def inverse_sbox_output(self):
        """Hamming weight of 1st round InvSBox (for decryption)"""
        return AES128_8bit(aes128_leakage.InvSBox_output)

    @property
    def last_round_state(self):
        """Hamming weight of 9th round state (InvSBox output)"""
        return AES128_8bit(aes128_leakage.LastroundHW)

    @property
    def after_key_mix(self):
        """Hamming weight of 1st round key mix (pt ^ key)"""
        return AES128_8bit(aes128_leakage.AfterKeyMixin)

    @property
    def mix_columns_output(self):
        """Hamming weight of 1st round mix columns"""
        return AES128_8bit(aes128_leakage.Mixcolumns_output)

    @property
    def inverse_sbox_output_alt(self):
        "ISbox output with correct key scheduling so it's highlighted in the jupyter callback"
        return AES128_8bit(aes128_leakage.InvSBox_output_alt)

    @property
    def shift_rows_output(self):
        """Hamming weight of shift rows output"""
        return AES128_8bit(aes128_leakage.ShiftColumns_output)

    @property
    def last_round_state_diff(self):
        """Hamming distance between rounds 9 and 10"""
        return AES128_8bit(aes128_leakage.LastroundStateDiff)

    @property
    def pipeline_diff(self):
        """Hamming distance between previous round 9 and current round 9 (for successive encryptions)"""
        return AES128_prev(aes128_leakage.PipelineDiff)

    @property
    def half_pipeline_diff(self):
        """Hamming distance between previous round 10 and current round 9 (for successive encryptions)"""
        return AES128_prev(aes128_leakage.HalfPipelineDiff)

    @property
    def last_round_state_diff_alternate(self):
        """Hamming distance between rounds 9 and 10 (alternate calculation)"""
        return AES128_8bit(aes128_leakage.LastroundStateDiffAlternate)

    @property
    def sbox_in_out_diff(self):
        """Hamming distance between SBox input and output."""
        return AES128_8bit(aes128_leakage.SBoxInOutDiff)

    @property
    def sbox_input_successive(self):
        """Hamming distance between 2 AES sbox inputs"""
        return AES128_8bit(aes128_leakage.SBoxInputSuccessive)

    @property
    def sbox_output_successive(self):
        """Hamming distance between 2 AES sbox outputs"""
        return AES128_8bit(aes128_leakage.SBoxOutputSuccessive)

    @property
    def round_1_2_state_diff_text(self):
        """Hamming distance between AES input and mix columns output"""
        return AES128_8bit(aes128_leakage.Round1Round2StateDiff_Text)

    @property
    def round_1_2_state_diff_key_mix(self):
        """Hamming distance between initial key mix and round 1 key mix"""
        return AES128_8bit(aes128_leakage.Round1Round2StateDiff_KeyMix)

    @property
    def round_1_2_state_diff_sbox(self):
        """Hamming distance between round 1 and round 2 sbox output"""
        return AES128_8bit(aes128_leakage.Round1Round2StateDiff_SBox)

    def __str__(self):
        """Structured glossary of all the different leakage models.

        The glossary includes the name of the leakage model and the
        description (doc string).
        """
        include = [
            'plaintext_key_xor',
            'sbox_output',
            'inverse_sbox_output',
            'last_round_state',
            'after_key_mix',
            'mix_columns_output',
            'shift_rows_output',
            'last_round_state_diff',
            'pipeline_diff',
            'half_pipeline_diff',
            'last_round_state_diff_alternate',
            'sbox_in_out_diff',
            'sbox_input_successive',
            'sbox_output_successive',
            'round_1_2_state_diff_text',
            'round_1_2_state_diff_key_mix',
            'round_1_2_state_diff_sbox',
            'inverse_sbox_output_alt',
            't_table',
            't_table_dec'
        ]
        models = [x for x in dir(self) if x in include]
        doc_strings = [getattr(self.__class__, x).__doc__ for x in models]

        items = []
        for model, doc_string in zip(models, doc_strings):
            if doc_string is None:
                items.append(model + ':')
            else:
                dedented_string = textwrap.dedent(doc_string)
                wrapped_string = '\n'.join(textwrap.wrap(dedented_string, 70))
                indented_doc_string = textwrap.indent(wrapped_string, ' ' * 2)
                items.append(':\n'.join([model, indented_doc_string]))

        return '\n\n'.join(items)
