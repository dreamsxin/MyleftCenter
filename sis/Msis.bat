@echo create sis file, please wait...

@createsis create MyleftCenter.pkg -cert fondo4.cer -key fondo4.key -pass 123456789qwerty

@rename MyleftCenter.sis "MyleftCenter.sisx"
@echo create sisx file success.