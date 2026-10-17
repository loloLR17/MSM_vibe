# R4-V — latence transactionnelle, mesure avant optimisation

Mission TR2-20261017-R4V-RTU-START-STOP-LATENCY. Aucun seuil d'arrêt industriel nouveau décidé. R0/B5/R1/R2/barrières/formats/anti-rejeu inchangés.

## Acquis et hypothèse testable

Captures R4U relues intégralement :1632ADU, START63,100s/STOP63,249s terminaux ; ACKSTOP→premierHELD55,757s. MCU instant d'effet et temps cumulés service/I/O non observés alors ; aucune attribution de63s à la FRAM seule. Le code fait des callbacks coopératifs avant/après chaque quantum32octets et dans CRC/copie/hash/recovery. Un drain implique SPI/status/config/horloge même si peu de FIFO est arrivée. Hypothèse : sur-service coûteux amplifie le temps des barrières ; à mesurer, pas une cause déjà prouvée.

## Composition et instrumentation

R4V explicite requiert recetteR4U/duplex4MHz, artéfact NON-PRODUCTION distinct, bootWAIT.96événements/32spans RAM bornés ; aucune impression ou GDB pendant transfert. MCU now_us(DWT suivi), événements capture observée après traitementRTU, callbackSTART/STOPdébut/fin, observation capteur, premier B3HELDpublié ; spans avant/après chaque pasR2 avec journal/root, service cumulé et I/O physique séparés. Dernier XYZ consommé/provider distinct du dernier XYZ produit (FIFO non lue lors du STOP). ACK/HELD/terminal maître mesurés séparément par captures COM4. B1 reste non supporté : aucune fausse publication de sa valeur. La capture vue aprèspoll est bornée par le service, pas datée comme l'arrivée électrique.

Profils comparables :
- baseline0 : mêmes callbacksFIFO à chaque checkpoint queR4U, instrumentation passive ; exige FRAM95da…9af41/journal46/root59/run7terminal, nouvelles clés5/5START/run8 et5/6STOP.
- candidat250 : même code et instrumentation, espacement seulement du drain coûteuxFIFO ; exige exactement snapshot aprèsbaseline prédit horslignefce8e1f9330989e02f7139d10cb066383bca1c58447a00a9852a8ca6c4b4a682/journal54/root67/run8terminal. Clés5/7START/run9 ; clé5/8STOP refusée une fois par budgetbench négatif, sans commit/effet ; puis5/9STOPneuf. Aucun budget positif fabriqué.

Le pacing reste désactivé par défaut. API seulement arrêtée, plafond un quart du work_limit existant, soit250µs. UART8événements et deadline restent traités à chaque checkpoint, publication10ms inchangée ; aucune barrière déplacée/supprimée. Borne conservative nominale avant qualification :250+1000+2500=3750µs<gap5000 ; les mesures réelles restent requises. Le service de sécurité ne dépend pas d'un reçu FRAM réussi. Toute erreur du foreground utilise le fault R2 existant (stop/invalidité) puis publie le snapshotB3invalide, sans reçu terminal fictif.

## Tests et résultats préalables

145tests complets PASS, incluant les144anciens/1561coupuresR2 ; nouveau pacing0/250, frontière horloge avec coût modèle4µs, refus du changement en acquisition, retry/collision, budgetSTOPabsent sans effet tardif, erreurFRAM, faute de service et B3invalid. Le contre-test faute de service échouait avant la correction de retraitB3, puis passe ; deux erreurs initiales d'oracle (lecturehorloge modèle4µs et retourprojectionB3enfaute) sont documentées, pas des contrôles produit affaiblis.4suitesO2/ASan/UBSan PASS6,50s, ARMcanonique/dédiés PASS. FirmwareV1/protocole1 conservé.

## Préflight et exécution conditionnelle

TR2-001 SN003500463235510B37333439/UID004A003D3842501820333157, M016WAIT/Flash2Mio c5bc…f5b26 et FRAM95da entière vérifiées fraîchement10:18:02UTC, capteurWHO7B/ODRoff/FIFObypass/TSoff. COM4FTDI0403:6001/BG04KSSW, COM3STLINK ; même raccordement confirméR4U, maître exclusifC# natif, aucunHP/SD.

Mesurebaseline nécessaire pour les jalons absents R4U, pas répétition du simplePASSfonctionnel. Après succès intégral et FRAM exacte au modèle, mesurer candidat ; sinon STOPsûr/archives, pas de nouvelle identité arbitraire. Ne jamais rejouer une clé admise ni retireR4T. Les modèles de métadonnées ne prouvent aucun effet physique à eux seuls.

Après probes, restaurer M016 par pages normales couvrant la plus grande image (calculer19pages selon taille réelle), relire2Mio et FRAM entière finale, vérifier capteur arrêté/detach/COM4fermé. Pas de rollbackFRAM/OB/RDP/masserase. RecoveredR2boot ne doit pas autoSTART ; gatesSHAexactes avant stores.

Aucune qualification physique R4V anticipée : objectif quantifié après diagnostic, pas de seuil industriel inventé. Si un arrêt rapide nécessite déplacer une barrière normative, décision humaine séparée obligatoire. R5/SQLite/Web et R6 restent hors qualification.
