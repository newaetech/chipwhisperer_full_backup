from chipwhisperer.analyzer.attacks.models import AES128_8bit as aes128_leakage
from chipwhisperer.analyzer.attacks.models.AES128_8bit import AES128_8bit as AES128_8bit
import textwrap


class EightBitAES128LeakageModels:
    """Contains the available leakage models for 8 bit AES128.

    To use a leakage model during a correlation power analysis attack
    see :func:`cpa <chipwhisperer.analyzer.cpa>`.

    :data:`leakage_models <chipwhisperer.analyzer.leakage_models>` is an instance of
    this class.
    """

    def new_model(self, model):
        return AES128_8bit(model)

    @property
    def plaintext_key_xor(self):
        """Plain text key XOR."""
        return AES128_8bit(aes128_leakage.PtKey_XOR)

    @property
    def sbox_output(self):
        """SBox Output."""
        return AES128_8bit(aes128_leakage.SBox_output)

    @property
    def inverse_sbox_output(self):
        """Inverse Sbox output."""
        return AES128_8bit(aes128_leakage.InvSBox_output)

    @property
    def last_round_state(self):
        """Last round state."""
        return AES128_8bit(aes128_leakage.LastroundHW)

    @property
    def after_key_mix(self):
        """After key mix operation."""
        return AES128_8bit(aes128_leakage.AfterKeyMixin)

    @property
    def mix_columns_output(self):
        """Output of the mix columns operation."""
        return AES128_8bit(aes128_leakage.Mixcolumns_output)

    @property
    def shift_columns_output(self):
        """Output of the shift columns operation."""
        return AES128_8bit(aes128_leakage.ShiftColumns_output)

    @property
    def last_round_state_diff(self):
        """Last round state using hamming distance."""
        return AES128_8bit(aes128_leakage.LastroundStateDiff)

    @property
    def last_round_state_diff_alternate(self):
        """Last round state using hamming distance, alternate."""
        return AES128_8bit(aes128_leakage.LastroundStateDiffAlternate)

    @property
    def sbox_in_out_diff(self):
        """Hamming distance between SBox input and output."""
        return AES128_8bit(aes128_leakage.SBoxInOutDiff)

    @property
    def sbox_input_successive(self):
        """Successive Sbox input."""
        return AES128_8bit(aes128_leakage.SBoxInputSuccessive)

    @property
    def sbox_output_successive(self):
        """Successive SBox output."""
        return AES128_8bit(aes128_leakage.SBoxOutputSuccessive)

    @property
    def round_1_2_state_diff_text(self):
        """Hamming distance between round 1 and 2 state using plain text."""
        return AES128_8bit(aes128_leakage.Round1Round2StateDiff_Text)

    @property
    def round_1_2_state_diff_key_mix(self):
        """Hamming distance between round 1 and 2 state during key mix
        operation.
        """
        return AES128_8bit(aes128_leakage.Round1Round2StateDiff_KeyMix)

    @property
    def round_1_2_state_diff_sbox(self):
        """Hamming distance between round 1 and 2 state during sbox lookup
        operation.
        """
        return AES128_8bit(aes128_leakage.Round1Round2StateDiff_SBox)

    def __str__(self):
        """Structured glossary of all the different leakage models.

        The glossary includes the name of the leakage model and the
        description (doc string).
        """
        non_callables = [x for x in dir(self) if not(callable(x)) and not x.startswith('__')]
        doc_strings = [getattr(self.__class__, x).__doc__ for x in non_callables]

        items = []
        for non_callable, doc_string in zip(non_callables, doc_strings):
            if doc_string is None:
                items.append(non_callable + ':')
            else:
                dedented_string = textwrap.dedent(doc_string)
                wrapped_string = '\n'.join(textwrap.wrap(dedented_string, 70))
                indented_doc_string = textwrap.indent(wrapped_string, ' ' * 2)
                items.append(':\n'.join([non_callable, indented_doc_string]))

        return '\n\n'.join(items)

