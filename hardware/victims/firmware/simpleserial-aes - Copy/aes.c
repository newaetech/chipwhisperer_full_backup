/* AES-128 Engine with state machine 
* A command MODE is taken as input by the aes function and executed.
* The command MODE can ask different operations at the same time but they have to be consistent.
* For example, asking for encryption and decryption at the same time is not possible
* More details about the possible commands in 'aes.h'
* Modifications: First version
;*---------------------------------------------------------------------------------------------------------------------
;* HISTORY:
;*----------------------------------------------------------------------------------------------------------------------
;* V 1.0 | 01/23/18 | First version  
;***********************************************************************************************************************
*/

#include "aes.h"
#include "string.h"
#include "platform.h"

/*
 * \brief  	AES engine for encrypting and decrypting protecting by affine masking 
 * \details The AES engine performs different commands. 
 *    
 * \param   Mode 			The mode is the command the user wants to execute.        
 * \param   struct_aes      The struct_aes is a strcuture that contains the AES context   
 * \param 	key 			The key is required when MODE_KEYINIT operation in part of the MODE command.
 * \param   input     		The input is required when MODE_ENC or MODE_DEC operation is part of the MODE command.
 * \param  	output 			The output is required when MODE_ENC or MODE_DEC operation is part of the MODE command.
 * \param  	random_aes 		An array of random values is required 
 * \return 	Error 			Error code: NO_ERROR if succeeds.
*/ 
UINT aes(UCHAR Mode, STRUCT_AES* struct_aes, const UCHARp key, const UCHARp input, UCHARp output, const UCHARp random_aes, const UCHARp random_key){
	UINT ret = NO_ERROR;
	UCHAR buf[19];

	local_memset(buf, 0, sizeof(buf));

	// Test if Mode asks for ENC and DEC at the same time or if the Mode as for no operation at all
	ret |= test_mode(Mode);
	if (ret != NO_ERROR){
		goto err;
	}

	// Test if ENC or DEC encryption operation is asked without AESINIT or KEYINIT
	ret |= test_mode_state(Mode, struct_aes->state);
	if (ret != NO_ERROR){
		goto err;
	}	

	// Test parameters
	ret |= test_parameter_content(Mode, key, input, output,random_aes,random_key);
	if (ret != NO_ERROR){
		goto err;
	}	
	
	// MODE_KEYINIT with external randomness
	if ((Mode & (MODE_KEYINIT|MODE_RANDOM_KEY_EXT)) == (MODE_KEYINIT|MODE_RANDOM_KEY_EXT)){
		local_memcpy(buf,random_key,19);								// Copy external randomness in random_key
	}
	else{
		if (get_random(buf, 19)!=NO_ERROR){						// Generate random value for key masking
			ret = ret | ERR_OP_GEN_RANDOM_KEY;					// Return an error if random generation fails
			goto err;
		}
	}

	if (Mode & MODE_KEYINIT){
		if (aes_loadKey(&struct_aes->key_context, key, buf)!=NO_ERROR){	// Key initialisation
			ret = ret | ERR_OP_KEYINIT;	
			goto err;
		}
		struct_aes->state = struct_aes->state| STATE_KEYINIT; 	// Update of state
		struct_aes->ctr_key = 0;								// Initialisation of the counter ctr_key
	}
	
	// MODE_AESINIT_ENC or MODE_AESINIT_DEC 
	if (Mode & MODE_RANDOM_AES_EXT){
		local_memcpy(buf,random_aes,19);
	}
	else{
		if (get_random(buf, 19)!=NO_ERROR){						// Generate random value for state masking
			ret = ret | ERR_OP_GEN_RANDOM_AES;					// return an error if random generation fails
			goto err;
		}
	}
	
	// MODE_AESINIT_ENC or MODE_AESINIT_DEC
	if (Mode & MODE_AESINIT_ENC){
		if (aes_init_enc(&struct_aes->aes_context, buf)!=NO_ERROR){		// Aes context initialisation
			ret = ret | ERR_OP_AESINIT;									// return an error if aes initialisation fails
			goto err;
		}		
		struct_aes->state = struct_aes->state| STATE_AESINIT_ENC; 		// Update of state 
		struct_aes->state = struct_aes->state & (~STATE_AESINIT_DEC); 	// Update of state: remove STATE_AESINIT_DEC from State 
		struct_aes->ctr_aes = 0;										// randomness used to mask the state is refreshed
	}

	// MODE_AESINIT_DEC
	if (Mode & MODE_AESINIT_DEC){
		if (aes_init_dec(&struct_aes->aes_context, buf)!=NO_ERROR){		// Aes context initialisation
			ret = ret | ERR_OP_AESINIT;									// return an error if aes initialisation fails
			goto err;
		}
		struct_aes->state = struct_aes->state| STATE_AESINIT_DEC; 		// Update of state: initialisation of aes decryption context 
		struct_aes->state = struct_aes->state & (~STATE_AESINIT_ENC); 	// Update of state: remove STATE_AESINIT_ENC from State 
		struct_aes->ctr_aes = 0;										// Initialisation of the counter ctr_aes
	}

	// MODE_AES_ENC or MODE_AES_DEC	
	switch (Mode & (MODE_ENC|MODE_DEC)){
		case MODE_ENC:
		if(aes_enc(&struct_aes->aes_context, &struct_aes->key_context, input,output)!=NO_ERROR){	// AES encryption
			ret = ret | ERR_OP_ENC;									// Return an error if aes encryption fails
			goto err;
		}
		struct_aes->ctr_key ++;										// Increment the counter under the same key randomness
		struct_aes->ctr_aes ++;										// Increment the counter under the same state randomness
		break;
		
		case (MODE_DEC):
		if(aes_dec(&struct_aes->aes_context, &struct_aes->key_context, input,output)!=NO_ERROR){	// AES decryption
			ret = ret |ERR_OP_DEC;										// Return an error if aes decryption fails
			goto err;
		}
		struct_aes->ctr_key ++;										// Increment the counter under the same key randomness
		struct_aes->ctr_aes ++;										// Increment the counter under the same state randomness
		break;

		default:
		break;
	}
err:
	return ret;
}


/*
 * \brief  		Test if the inputs are consistent with MODE.
 * \details 	This function checks if the inputs are consistent with MODE. For example, MODE cannot contain ENC and DEC commands.  
 *    
 * \param   Mode 			The mode is the command the user wants to execute.          
 * \param 	key 			The key is required when MODE_KEYINIT operation in part of the MODE command.
 * \param   input     		The input is required when MODE_ENC or MODE_DEC operation is part of the MODE command.
 * \param  	output 			The output is required when MODE_ENC or MODE_DEC operation is part of the MODE command.
 * \param  	random_aes 		An array of random values is required
 * \return 	Error 	Error code: NO_ERROR if succeeds.
 */
UINT test_parameter_content(UCHAR Mode, const UCHARp key,  const UCHARp input, UCHARp output, const UCHARp random_aes, const UCHARp random_key){

	if (Mode & MODE_KEYINIT){
		if(key== NULL){
			return ERR_KEY_MISSING;
		}	
	}

	if ( (Mode & (MODE_KEYINIT|MODE_RANDOM_KEY_EXT)) == (MODE_KEYINIT|MODE_RANDOM_KEY_EXT)){
		if(random_key== NULL){
			return ERR_RANDOM_KEY_MISSING;
		}	
	}	

	if ( Mode & MODE_RANDOM_AES_EXT){
		if(random_aes== NULL){
			return ERR_RANDOM_AES_MISSING;
		}	
	}
	
	if ( (Mode & MODE_ENC) == MODE_ENC){
		if((input== NULL) || (output==NULL)){
			return ERR_INPUT_MISSING;
		}
	}

	if ( (Mode & MODE_DEC) == MODE_DEC){
		if((input== NULL) || (output==NULL)){
			return ERR_INPUT_MISSING;
		}
	}

	return NO_ERROR;
}


/*
 * \brief  			Test if the Mode is meaningfull.
 * \details 		This function checks if the MODE is consistent. For example, MODE cannot contain ENC and DEC commands at the same time.  
 * \param   Mode 	The mode is the command the user wants to execute.        
 * \return 	Error 	Error code: NO_ERROR if succeeds. 
 */
UINT test_mode(UCHAR Mode){

	// test if Mode asks for ENC and DEC operations at the same time
	if ((Mode & (MODE_ENC|MODE_DEC) ) == (MODE_ENC|MODE_DEC) ){
		return ERR_ENC_AND_DEC;
	}

	// test if Mode asks for ENC and DEC AESINIT operations at the same time
	if ((Mode & (MODE_AESINIT_ENC|MODE_AESINIT_DEC)) == (MODE_AESINIT_ENC|MODE_AESINIT_DEC) ){
		return ERR_ENC_AND_DEC_AESINIT;
	}	

	// test if Mode asks for ENC and DEC AESINIT operations at the same time or DEC and ENC AESINIT operations at the same time
	if (((Mode & (MODE_AESINIT_ENC|MODE_DEC)) == (MODE_AESINIT_ENC|MODE_DEC)) || ((Mode & (MODE_AESINIT_DEC|MODE_ENC)) == (MODE_AESINIT_DEC|MODE_ENC))){
		return ERR_AESINIT_AND_OP;
	}	
	// test if Mode asks for no operation
	if (Mode == MODE_UNSPECIFIED){
		return ERR_NO_OPERATION;
	}

	return NO_ERROR;

}


/*
 * \brief  	AES engine for encrypting and decrypting protecting by affine masking 
 * \details It check if the command MODE asked by the user is consistent with the current State. 
 *			For example, if the user ask for en encryption(MODE = MODE_ENC) but the key context or the aes context were not initialized 
 *			then it returns the corresponding error (respectively ERR_KEYINIT_MISSING or ERR_AESINIT_MISSING)  
 * \param   Mode 			The mode is the command the user wants to execute.        

 * \param   State 			The State is contained the initialized contexts (STATE_KEYINIT,STATE_AESINIT_ENC,STATE_AESINIT_DEC)
 * \return  TODO
 */
UINT test_mode_state(UCHAR Mode, UCHAR State){

	if((Mode & MODE_ENC)== MODE_ENC){					// User asks for an encryption

		if (((State & STATE_KEYINIT) != STATE_KEYINIT)&((Mode & MODE_KEYINIT)== 0)){ // Key not loaded and not int Mode
			return ERR_KEYINIT_MISSING;
		}

		if (((State & STATE_AESINIT_ENC) != STATE_AESINIT_ENC)&((Mode & MODE_AESINIT_DEC)== MODE_AESINIT_DEC)){	// AES not initialized and init_aes not in Mode
			return ERR_AESINIT_BAD;
		}

		if (((State & STATE_AESINIT_ENC) != STATE_AESINIT_ENC)&((Mode & MODE_AESINIT_ENC) != MODE_AESINIT_ENC)){	// AES not initialized and init_aes not in Mode
			return ERR_AESINIT_MISSING;
		}
	}

	if((Mode & MODE_DEC)== MODE_DEC){					// User asks for an decryption

		if (((State & STATE_KEYINIT) != STATE_KEYINIT)&((Mode & MODE_KEYINIT)!= MODE_KEYINIT)){ // Key not loaded and not int Mode
			return ERR_KEYINIT_MISSING;
		}

		if (((State & STATE_AESINIT_ENC) == STATE_AESINIT_ENC) & ((State & STATE_AESINIT_DEC) != STATE_AESINIT_DEC)){	// LOK TODO
			return ERR_AESINIT_BAD;
		}

		if (((State & STATE_AESINIT_DEC) != STATE_AESINIT_DEC)&((Mode & MODE_AESINIT_DEC) != MODE_AESINIT_DEC)){	// AES not initialized and init_aes not in Mode
			return ERR_AESINIT_MISSING;
		}

	}
	return NO_ERROR;
}
