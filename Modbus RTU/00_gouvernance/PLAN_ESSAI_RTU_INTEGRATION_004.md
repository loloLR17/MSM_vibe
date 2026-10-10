# Mission 004 — composition applicative et essai RS-485 préparé

Date : 10 octobre 2026. Ce plan n'a pas été exécuté sur matériel.

## Chaîne logicielle livrée

`modbus_system_server` emprunte un `SystemRuntime` déjà booté et réutilise P12-D/B/H/C/E : événements → délimitation → longueur/CRC → adresse → actualisation des autorités applicatives → PDU → ADU → transport. L'actualisation n'est déclenchée qu'après acceptation CRC/adresse, jamais dans une ISR. B0 vient d'une identité explicitement fournie ; son absence produit une exception 04. B1/B3/B7 utilisent les projections disponibles du runtime. B2 lit l'autorité `TimeService` actuelle et accepte le staging via l'adapter existant. B4 peut utiliser un `ConfigurationWorkflow` fourni : les écritures invalident sa validation, les lectures reflètent son staging et son état. Sans workflow, les lectures du runtime restent disponibles et les écritures renvoient 04. B6 utilise la même image pour lecture et sélection d'inventaire.

B5 utilise le mailbox/engine du runtime et un port `ModbusCommandSubmit` explicite. Il reçoit une soumission capturée ou invalide, avec son identité réelle. Un gestionnaire doit conserver les invariants V1 de refus fonctionnel, admission, retry/collision, annulation et persistance ; une erreur d'infrastructure est remontée au PDU. Il doit publier les résultats métier dans B5 et renvoyer TR2_OK pour un refus fonctionnel traité (notamment ID 0 → code 14). Sans gestionnaire, aucune écriture B5 n'est reconnue comme appliquée : exception 04, mailbox inchangé. La mission ne fournit pas un dispatcher universel des commandes 1..11 ; l'intégration hôte démontre une soumission jusqu'à l'exécuteur de production REFRESH_INDICATORS. Elle vérifie aussi la transmission d'une soumission ID 0 au port ; ce test ne qualifie pas son refus fonctionnel en production.

Tous les objets pointés doivent rester vivants. Ne pas copier/déplacer un serveur initialisé : ses callbacks et projections pointent vers son propre stockage. `start()` est explicite et son échec remonte. La perte de readiness invalide la réception et arrête le serveur ; le propriétaire doit effectuer une récupération explicite (dont purge des événements transport obsolètes) avant redémarrage. Un échec TX remonte sans rejouer automatiquement une commande déjà traitée. Le transport doit posséder une copie du buffer TX pendant son émission asynchrone, comme le transport STM32 actuel. Les services métier restent exécutés synchroniquement ; aucune garantie de débit ou de temps de réponse sur cible n'est déduite des tests hôte.

## Raccordement STM32 encore requis

`main.c` appelle `stm32_modbus_application_bind()` après initialisation UART. Le hook faible livré renvoie explicitement `TR2_ERROR_NOT_AVAILABLE`, observable dans `tr2_modbus_application_binding_result` : le harness conserve alors le diagnostic de réception 003. Aucun paramètre électrique ou service de production n'est simulé. Un override de carte doit :

1. Fournir un SystemRuntime réellement initialisé/booté avec les dépendances durables prévues : clocks/continuité, FRAM transactionnelle, stockage campagne, vibration, environnement de validation configuration et autorités optionnelles requises. Les instances temporaires des séquences de qualification actuelles ne constituent pas ce runtime de production.
2. Fournir une adresse locale provisionnée 1..247. P12-H0 impose une valeur explicite mais n'en fixe ni valeur production ni source. Une identité B0 doit provenir d'un provisionnement traçable.
3. Fournir un transport série avec stratégie half-duplex qualifiée. Le transport UART PG7/PG8 existant ne pilote pas DE//RE ; le fournir directement ne satisfait pas ce prérequis. L'override peut l'envelopper, mais doit gérer direction, fin physique de TX et réception/écho sans perdre les frontières RTU.
4. Fournir les workflow/gestionnaires applicatifs activés, avec durée de vie permanente. Le hook renvoie TR2_OK seulement lorsque le raccordement est réel. Une erreur différente de NOT_AVAILABLE provoque Error_Handler plutôt qu'une dégradation silencieuse.

Le harness bascule alors sur `modbus_system_server_start/poll_once`, avec heartbeat non bloquant et diagnostics `tr2_modbus_application_last_poll_result` / `tr2_modbus_application_poll_error_count`. Aucun consommateur diagnostique parallèle de la même file n'est lancé. Le build livré reste par défaut en diagnostic RX, donc **n'émet pas de réponse applicative sur cible**.

## Prérequis physiques à solder avant essai

Références : `FREEZE_FIRMWARE_P12F_STM32_LPUART1.md`, `FREEZE_FIRMWARE_P12G_STM32_RTU_TIMING.md`, `ARBITRAGE_FIRMWARE_P12H0_RTU_SERVER_ADDRESSING.md`, `CONCEPTION_FIRMWARE_P12H3H_B2_NUCLEO_PHYSICAL_OCCUPANCY.md`.

- Profil fixé : 115200 / 8E1, PG7 TX = CN12-67, PG8 RX = CN12-66 ; TIM6 nominal 750/1750 µs. Ce sont des décisions/documentations existantes, pas une nouvelle mesure.
- DE//RE : PG5 (CN12-68) et PG4 (CN12-69) ne sont que des candidats réservés ; affectation et stratégie à confirmer humainement. Aucun câblage de ces broches n'est prescrit par ce plan.
- Le freeze historique cite EVAL-ADM2587EARDZ, le plan d'occupation cite Click ADM2867E. Identifier le transceiver réellement installé et ses broches/alimentations, polarité DE//RE, masses/isolement, terminaison et polarisation à partir du montage/documentation correspondants. Ne pas confondre les cartes.
- Domaine VDDIO2 PG4..PG8 à vérifier et qualifier avant connexion. Le code actuel ne contient pas d'activation VDDIO2 explicite ; aucun correctif électrique n'est déduit sans décision de carte.
- Choisir et identifier l'adaptateur USB RS-485 réel, avec commande automatique de direction et echo maîtrisé. Le VCP ST-LINK n'est pas cet adaptateur. Tester sur un bus isolé à un seul esclave ; l'adresse utilisée dans le cas other-address doit être libre.
- Préparer un artefact applicatif raccordé, son SHA et ses empreintes, puis une mission autorisant explicitement flash/debug. Les séquences E1/E4 existantes peuvent écrire au boot : contrôler la microSD et les conditions de la procédure qualifiée avant tout flash. E4 reste ouverte, aucun essai de power-loss dans ce plan.

## Commandes préparées — Debian, après autorisation matérielle distincte

Le générateur offline ne touche aucun port et réutilise le codec C du firmware. La commande de validation complète construit `build-host-validation/tr2_rtu_request`. Exemple de préparation sans matériel (17 est une adresse de test, pas une valeur production) :

```bash
"Modbus RTU/05_Firmware/build-host-validation/tr2_rtu_request" 17 b1
```

Avant l'essai, définir `RS485_PORT` avec le périphérique Linux réel et `TR2_UNIT_ID` avec l'adresse effectivement provisionnée ; les commandes suivantes émettent sur le bus et ne sont pas exécutées par la mission 004. Vérifier la disponibilité de `stty`, `timeout`, `cat`, `xxd`.

```bash
: "${RS485_PORT:?Périphérique USB RS-485 réel requis}"
: "${TR2_UNIT_ID:?Adresse provisionnée requise}"
RTU_REQUEST="$PWD/Modbus RTU/05_Firmware/build-host-validation/tr2_rtu_request"
stty -F "$RS485_PORT" raw -echo 115200 cs8 parenb -parodd -cstopb -ixon -ixoff
stty -F "$RS485_PORT" -a

# Exécuter séparément pour b1, unsupported, bad-crc, other-address.
RTU_CASE=b1
"$RTU_REQUEST" "$TR2_UNIT_ID" "$RTU_CASE" > /tmp/tr2-request.hex
: > /tmp/tr2-response.bin
timeout 1s cat "$RS485_PORT" > /tmp/tr2-response.bin &
reader_pid=$!
xxd -r -p /tmp/tr2-request.hex > "$RS485_PORT"
if wait "$reader_pid"; then
    receive_status=0
else
    receive_status=$?
    # 124 signifie fin normale de la fenêtre de capture, pas preuve de réponse.
    test "$receive_status" -eq 124 || exit "$receive_status"
fi
xxd -p /tmp/tr2-response.bin
# Pour les cas avec réponse : décodage longueur/CRC via le codec réel.
"$RTU_REQUEST" --decode /tmp/tr2-response.bin
```

Pour bad-crc et other-address, le fichier attendu est vide : vérifier `test ! -s /tmp/tr2-response.bin` et ne pas exécuter --decode. Capturer chaque cas séparément ; un écho ou plusieurs trames concaténées doivent être analysés, pas interprétés comme une réponse valide.

## Observations et critères de réussite

| Cas | Résultat requis |
|---|---|
| b1 : FC03, adresse 1000, quantité 1 | Une ADU à l'adresse locale, CRC valide, PDU `03 02 xx xx`, valeur égale au B1 de l'application réellement prête. Une exception 04 ne satisfait pas ce critère. |
| unsupported : FC06 | Une ADU CRC valide, PDU `86 01`, aucun effet métier. |
| bad-crc | Aucune réponse, aucun effet métier. |
| other-address | Aucune réponse, aucun effet métier. |
| Trame coupée entre T1.5 et T3.5 (générateur/analyseur qualifié) | Pas de traitement de cette trame ; la suivante correctement délimitée est reçue. |
| Direction et timing instrumentés | DE actif pendant la réponse et relâché après le dernier stop bit ; aucune collision/auto-réception parasite ; mesure T1.5/T3.5 conforme au profil gelé. |

Conserver captures, compteurs, traces instrumentées et identification de l'artefact. Répéter les quatre premiers cas, puis une requête valide après chaque rejet pour démontrer la reprise. Le taux de répétition et le turnaround doivent être qualifiés séparément : aucun débit industriel n'est affirmé ici. Compilation, tests hôte et encodage offline ne remplacent aucune de ces observations.
