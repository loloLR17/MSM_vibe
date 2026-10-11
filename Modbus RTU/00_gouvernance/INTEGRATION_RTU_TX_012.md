# Réponse RTU progressive — audit et tranche012

11 octobre2026. **IMPLEMENTATION HÔTE VALIDÉE / COMPILÉ STM32 / NON FLASHÉ / TX PHYSIQUE BLOQUÉ.** Acquis RX011 conservé, pas de nouvel essai.

## Audit et raccordement minimal

Chaîne existante : `main → stm32_modbus_application_bind → ModbusSystemServer → ModbusRtuServerRuntime → récepteur/CRC/adresse → ModbusPduServer V1 → encodeurADU → SerialTransport.transmit → stm32_serial_transport`.

Le binding faible `stm32_production_binding_acquire` renvoie NOT_AVAILABLE. Le propriétaire durable exige géométrieFRAM, image transactionnelle récupérée valide, CampaignDataStore réel et métadonnées/configuration pertinentes ; EMPTY n'est pas formaté implicitement. Adresse1..247 et B0 doivent venir du provisioning. Aucun recyclage des fixturesE4, runtime fabriqué ou adresse1 imposée. Le chemin applicatif main existe déjà : le plus petit manque transport est DE/TC/retourRX ; activation carte/production reste un jalon séparé.

Mapping V1, types sans float, CRC, filtrageunitid, FC03/FC16, broadcast sans réponse, exceptions et sémantiqueB5 inchangés. Les freezesP12F/G documentent historiquement l'absenceDE, P12H gèle le serveur portable ; cette extension est traçable ici sans modifier ces documents historiques ni B0..B7.

IRQ UART/TIM6 à priorité5, file64 événements (63 utilisables), bufferTX256 octets, pasDMA. P12G : timer1MHz, T1.5=750µs, T3.5=1750µs. Overflow purge et événementERROR conservés. Serveur synchrone main aprèsT3.5, jamais appelé enISR. Mutation métier peut précéder échecTX suivant contrat serveur existant : aucun replay automatique.

## Nouvelle séquence transport

Gate CMake `TR2_STM32_RS485_TX_QUALIFIED=OFF` par défaut, macro0 dans header. Même un binding applicatif fourni ne peut faire émettre ce transport dans cette configuration. Le build alternatif ON compile la branche mais **ne qualifie pas la carte et ne fournit pas le binding de production**.

Transmission acceptée seulement après consommationT3.5, file vide, aucun RXNE ou BUSY matériel, timeridle, aucun TX ou défautlatched. Vérification et armement sous sectioncritique PRIMASK restauré ; ISRnouveloctet invalide le silence. Cette défense évite une réponse retardée quand une nouvelle requête arrive, sans garantir l'absence de collision électrique/multimaître.

1. Copier réponse du serveur dans buffer borné ; abortReceive HAL, sans DMA.
2. PG4DE haut (J2 commun implique /RE haut), TIM6watchdog50ms, UARTTransmit_IT.
3. Ignorer callbackRX tardif/écho pendantTX ; conserverDEhaut aprèsTXE/bufferempty.
4. CallbackHAL **TC** : DEbas d'abord, nettoyage/réarmementRX, garde intertrame1750µs. Garde ne synthétise ni requête ni autorisationTX ; prochaine réponse requiert un vrai T3.5RX.
5. ÉchecstartTX/abortRX, erreur pendantTX, timeout ou restaurationRX défaillante : **DEbas avant abortTX**, timerarrêté, réarmementRX tenté, erreurfile, TXfaultlatched jusqu'à nouvelleinitialisation. Aucun retry/re-enable silencieux. ErreurRX ordinaire réarmeHAL proprement ; réarmementoctet en échec devient erreur/fault visible.

HALCubeU5v1.9.0 auditée : UART_IRQHandler testeTC/TCIE avant UART_EndTransmit_IT qui appelleTxCpltCallback ; TXE est distinct. AbortReceive/AbortTransmit sont opérations registre bornées dans cette configuration sansDMA ; mockhôte teste le sourceSTM32, pas l'électronique ni l'implémentation interneHAL.

## Conditions physiques et limites

[Schéma Clickv102](https://download.mikroe.com/documents/add-on-boards/click/rs485_lsolator_2_click/rs485-isolator-2-click-schematic-v102.pdf) relu008 : J2DE–/RE, J3/RE–RST, VIO=VCCsélectionnée. PG4DE et pontJ2soudé rapportés ; PG5 et /RE sans fil. Réception011 et IDRPG4bas acquis, mais continuité réelleJ2/révision, /RE haut lorsDEhaut et VIOClick3,31V non établis pourTX. Ne pas inférer sûretéTX de RXréussi. [ADM2867E RevD table2](https://www.analog.com/media/en/technical-documentation/data-sheets/adm2867e_adm2865e_adm2863e_adm2861e.pdf) : enable/disableDEmax40ns dans conditions constructeur. Il existe des opérationsHAL/TIM entreDEhaut et démarrageUART, mais aucun délai électrique mesuré revendiqué ; mesurer direction/turnaround sur banc avant qualification.

DEbas au boot via initialiseur008 (latchavantoutput/IO2SV), bas sur erreurs accessibles. **Intervalle reset avant initialiseur non garanti matériellement** ; watchdog50ms dépend de TIM6/IRQ opérationnels, ne protège pas une panneCPU/interruptions masquées. Les octets d'une violation du silence postTX ne constituent pas un essai de conformité qualifié. Pull-down logiciel et absencecollision ne valent pas garantie matérielle permanente ; toute évolution physique demeure décision utilisateur, pas un changement de pont automatique.

**Décision012 : aucun flash/debug/émission.** Permission disponible mais condition physique non acquise ; BIN008 conservé sur carte. COM7HP inaccessible à Codex, aucune nouvelle émission demandée. Artefacts008 exacts sauvegardés horsGit avant build012 ; ne pas utiliser ELF012 pour debug du BIN008.

## Étape suivante minimale préparée

Conserver SB28/SB29 et câblage existants, pas de nouveau testRX. Rassembler seulement les faits encore absents pourTX : révision/pontDE–/RE effectivement commun sans commandeRSTconcurrente, VIOClickcompatible3V3 et /RE conforme àDE. Les observations existantes ne sont pas redemandées. Mesure/instrumentation dynamiqueDE–TXD–bus pour TC/turnaround à planifier lors du jalonTX autorisé ; pas de manipulation en012.

Fournir ensuite binding carte explicite et validé (adresse/B0/dépendances/runtime/stockage), sans toucherE4 ni inventer de format. Après gates réunies : build qualifiéON de sources exactement revues, sauvegardeFlash2Mio/SN/hash, flashstandard à0x08000000/vérification/relecture, ELFcorrespondant, essai requête/réponse unique contrôlé avec HP. Plan attendu : DEbas repos → haut avant premierstartbit → haut jusqu'au dernierstopbit(TC) → bas/RX ; réponseV1 etCRC exacts, défautforcebas, aucunTXnon sollicité. Une nouvelle émission HP nécessite accès contrôlé et contrat ; pas de commande exécutable livrée comme essai acquis.

Tests012 relient le **vrai ModbusSystemServer V1** au **vrai adaptateur STM32** sur HALsimulée, runtime/B0 clairement fixtureshôte (adresse17, valeur0x1234), jamais provisioningcarte : réponsesCRC, exceptionFC06, CRCinvalide, adresseétrangère, broadcasts, interoctetsinterrompus, aucuneémissionavantT3.5, activitéplusrécente/BUSY/RXNE, fileoverflow, busy, DE/TC/retourRX, timeout50ms, erreurs/rearmRX, faultlatched, écho, readinessperdue, PRIMASKpréservé et gateOFF. Tests existants production/mapping inchangés.
