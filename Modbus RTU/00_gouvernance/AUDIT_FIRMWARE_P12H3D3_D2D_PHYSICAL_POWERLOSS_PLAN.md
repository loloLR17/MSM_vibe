# P12-H3d3-D2-D — Plan de qualification power-loss physique H3d2 / FRAM

## Statut

**PLAN DE QUALIFICATION — EN COURS**

Cette tranche part du gel D2-C et ne modifie pas les invariants H3d2.

## 1. Objectif

Qualifier sur STM32U575 + FRAM réelle le comportement de recovery après une coupure d'alimentation réelle à des points contrôlés du protocole de commit H3d2.

La baseline physique de départ est :

- autorité `VALID` ;
- génération 2 ;
- image B ;
- payload logique offset 0 = `0xA5`.

La génération candidate suivante sera donc génération 3 sur image A.

## 2. Principe de sécurité

Aucune coupure ne sera faite « au hasard ».

Un adaptateur de qualification, limité à la composition STM32, interceptera les appels `TransactionalImagePhysicalStorage.write` sans modifier le moteur transactionnel portable H3d2 ni le driver FRAM.

À un point de coupure armé, l'adaptateur :

1. exécute exactement la partie physique prévue pour le scénario ;
2. place une sonde volatile explicite ;
3. donne un signal visible de qualification ;
4. se bloque volontairement avant que le moteur H3d2 puisse poursuivre ;
5. l'opérateur coupe alors réellement l'alimentation ;
6. le firmware de qualification est ensuite désarmé avant l'observation de recovery lorsque le scénario l'exige.

Le blocage logiciel n'est pas lui-même la coupure qualifiée : la qualification porte sur la perte réelle d'alimentation qui suit.

Aucun retry automatique n'est autorisé.

## 3. Points de coupure retenus

L'ordre H3d2 gelé est :

1. payload complet de l'image inactive ;
2. header final de cette image ;
3. validation complète de l'image ;
4. superbloc de publication opposé ;
5. validation de la publication ;
6. bascule d'autorité runtime.

La campagne physique sera progressive.

### D2-D1 — payload image inactive partiellement écrit

Écrire seulement un préfixe contrôlé du payload de l'image A, puis bloquer avant la fin de l'appel physique.

Après coupure réelle et reboot, l'ancienne publication génération 2 / image B doit rester autorité et restituer `0xA5`.

Ce scénario vérifie qu'une image inactive partiellement réécrite sans header final/publication ne remplace pas l'autorité précédente.

### D2-D2 — payload complet, avant header final

Laisser l'écriture du payload de l'image A se terminer, puis bloquer avant l'écriture du header final.

Après coupure/reboot, génération 2 / image B doit rester autorité.

### D2-D3 — image génération 3 finalisée et validable, avant publication

Laisser terminer payload + header image A, puis bloquer avant l'écriture du nouveau superbloc.

Après coupure/reboot, génération 2 / image B doit rester autorité : une image nouvelle non publiée n'est pas autorité.

### D2-D4 — publication physiquement écrite

Laisser écrire le nouveau superbloc génération 3 / image A puis provoquer la coupure avant retour normal complet du scénario de qualification.

Après reboot, recovery peut légitimement sélectionner la nouvelle autorité si le superbloc et l'image sont physiquement valides. L'observation doit être décrite factuellement ; aucune réussite ne sera inférée d'un retour logiciel qui n'a pas eu lieu.

## 4. Mutation de campagne

Pour distinguer sans ambiguïté ancienne et nouvelle autorité :

- ancienne valeur connue : offset 0 = `0xA5` ;
- valeur candidate D2-D : offset 0 = `0x5A`.

Résultats attendus :

- autorité ancienne : génération 2 / image B / `0xA5` ;
- autorité nouvelle publiée : génération 3 / image A / `0x5A`.

Chaque scénario devra repartir d'une baseline explicitement observée. Aucun scénario suivant ne sera lancé sur une baseline supposée.

## 5. Instrumentation

L'instrumentation doit rester hors du moteur portable `transactional_image_media.c`.

Elle doit :

- être compilée uniquement derrière une porte de qualification explicite ;
- conserver le backend FRAM réel ;
- intercepter uniquement le contrat physique `read/write` ;
- ne pas modifier le format physique ;
- ne pas fabriquer de succès H3d2 ;
- exposer des sondes GDB du scénario armé et du point atteint ;
- fournir un signal LED distinct du clignotement normal avant coupure ;
- être désarmée après chaque scénario avant toute conclusion de recovery.

## 6. Critères D2-D1

Première tranche à implémenter :

1. vérifier baseline `VALID / gen2 / B / 0xA5` ;
2. préparer candidate offset 0 = `0x5A` ;
3. lancer un commit unique ;
4. intercepter l'écriture du payload vers l'image inactive A ;
5. écrire un préfixe strictement inférieur à 51 818 octets ;
6. signaler le point atteint et bloquer ;
7. couper réellement l'alimentation ;
8. désarmer le scénario ;
9. recompiler/flasher sans toucher à la FRAM ;
10. reboot/recovery ;
11. exiger `VALID / gen2 / B / 0xA5`.

## 7. Résultats physiques acquis

### D2-D1 — payload partiel

D2-D1 est **qualifié physiquement**. Depuis la baseline `VALID / gen2 / B / 0xA5`, le backend FRAM a écrit avec succès un préfixe contrôlé de 4096 octets du payload de l'image inactive A. Le point de coupure a été confirmé par GDB, puis une perte réelle d'alimentation a été effectuée.

Après désarmement, reprogrammation sous reset et reboot, le recovery a donné `TR2_OK / VALID / gen2 / B / 0xA5`. L'écriture partielle de l'image inactive n'a donc pas remplacé l'autorité publiée.

### D2-D2 — payload complet, avant header final

D2-D2 est **qualifié physiquement**. Depuis la baseline explicitement observée `VALID / gen2 / B / 0xA5`, les sondes avant coupure ont confirmé :

- recovery initial : `TR2_OK / VALID / gen2 / B` ;
- écriture D2-D2 tentée : oui ;
- écriture physique des 51 818 octets du payload : `TR2_OK` ;
- commit tenté : oui ;
- point de coupure atteint : oui.

L'adaptateur a immobilisé le CPU après le retour `TR2_OK` de l'écriture du payload complet et avant retour au moteur H3d2, donc avant l'écriture du header final génération 3. Une perte réelle d'alimentation a alors été effectuée.

Après désarmement, reprogrammation sous reset et reboot, les sondes ont donné :

- stockage : `TR2_OK` ;
- géométrie : `TR2_OK` ;
- média : `TR2_OK` ;
- recovery : `TR2_OK` ;
- statut : `VALID` ;
- génération : 2 ;
- image active : B ;
- payload logique offset 0 : `0xA5`.

Conclusion limitée à D2-D2 : un payload candidat entièrement écrit dans l'image inactive A, sans son nouveau header final ni publication, n'a pas remplacé l'autorité publiée génération 2 / image B.

### D2-D3 — première tentative invalide, état persistant observé

La première tentative D2-D3 n'est **pas une qualification D2-D3**. L'adaptateur de qualification avait été configuré par erreur pour intercepter le superbloc B alors que, depuis la baseline génération 2 publiée par B, H3d2 publie la génération suivante par le superbloc opposé A.

Observations avant diagnostic :

- recovery initial : `TR2_OK / VALID / gen2 / B` ;
- candidate offset 0 : `0x5A` après mutation RAM ;
- écriture candidate : `TR2_OK` ;
- commit tenté : oui ;
- payload image A complet : oui ;
- header final image A : oui ;
- point d'interception D2-D3 : non atteint.

La carte est donc revenue au fonctionnement normal (LD1 clignotant). Aucune coupure d'alimentation D2-D3 n'a été effectuée.

Après désarmement et correction de l'adaptateur vers le superbloc A, un firmware non destructif a été reprogrammé. Le recovery suivant a observé :

- stockage / géométrie / média / recovery : `TR2_OK` ;
- statut : `VALID` ;
- génération : 3 ;
- image active : A ;
- payload logique offset 0 : `0x5A`.

Conclusion : la tentative invalide avait entièrement publié le commit génération 3 / image A. Ce résultat est conservé comme diagnostic de campagne, pas comme preuve D2-D3. La prochaine tentative D2-D3 doit repartir de cette nouvelle baseline explicitement observée et adapter dynamiquement ou explicitement l'image inactive et le superbloc de publication attendus.

### D2-D3 — image candidate finalisée avant publication : qualifié physiquement

Baseline physique observée avant scénario : `VALID / gen3 / image A / payload[0] 0x5A`.

Candidate : génération 4, image B, payload logique offset 0 muté à `0xA6`. L'adaptateur de qualification, dérivé de l'autorité récupérée, a laissé s'effectuer le payload complet et le header final de l'image inactive puis a intercepté l'écriture du superbloc de publication opposé avant de déléguer le moindre octet à la FRAM.

Preuve pré-coupure :

- écriture logique tentée : oui ;
- résultat écriture logique : `TR2_OK` ;
- commit tenté : oui ;
- payload image B complet : oui ;
- header final image B complet : oui ;
- point de coupure avant publication : atteint.

Une coupure physique réelle de l'alimentation a alors été effectuée. Après désarmement, validation complète du firmware, reprogrammation sous RESET et redémarrage, le recovery a observé :

- stockage / géométrie / média / recovery : `TR2_OK` ;
- statut : `VALID` ;
- génération : 3 ;
- image active : A ;
- payload logique offset 0 : `0x5A`.

Conclusion D2-D3 : **qualifié physiquement**. Une image candidate génération 4 physiquement complète et finalisée mais non publiée ne devient pas l'autorité après coupure ; le recovery conserve l'autorité publiée génération 3 / image A.


### D2-D4 — publication physiquement écrite : qualifié physiquement

Baseline physique observée avant scénario : `VALID / gen3 / image A / payload[0] 0x5A`.

Candidate : génération 4, image B, payload logique offset 0 muté à `0xA6`. L'adaptateur de qualification, dérivé de l'autorité récupérée, a laissé s'effectuer le payload complet, le header final et la validation de l'image inactive, puis a délégué l'écriture complète du superbloc de publication opposé au backend FRAM réel. Le CPU a été immobilisé immédiatement après le retour `TR2_OK` de cette écriture physique, avant retour au moteur H3d2.

Preuve pré-coupure :

- recovery initial : `TR2_OK / VALID / gen3 / image A` ;
- écriture logique tentée : oui ;
- résultat écriture logique : `TR2_OK` ;
- commit tenté : oui ;
- payload image B complet : oui ;
- header final image B complet : oui ;
- publication physique du superbloc opposé : complète, `TR2_OK` ;
- point de coupure après publication : atteint.

Une coupure physique réelle de l'alimentation a alors été effectuée. Après désarmement, validation complète du firmware, reprogrammation sous RESET et redémarrage, le recovery a observé :

- stockage / géométrie / média / recovery : `TR2_OK` ;
- statut : `VALID` ;
- génération : 4 ;
- image active : B ;
- payload logique offset 0 : `0xA6`.

Conclusion D2-D4 : **qualifié physiquement**. Lorsque la nouvelle image est complète et que son superbloc de publication a été physiquement écrit avec succès avant la perte réelle d'alimentation, le recovery sélectionne la nouvelle autorité publiée génération 4 / image B, même si le commit n'a pas pu retourner normalement à son appelant avant la coupure.

## 8. Ce qui n'est pas encore qualifié

Ne sont pas encore qualifiés :

- D2-D4 ;
- timing exact de la coupure ;
- brown-out ou rampes d'alimentation ;
- comportement électrique du bus pendant la chute de tension ;
- coupure pendant une transaction SPI individuelle ;
- endurance FRAM ;
- fault injection SPI.

## 9. Règle de progression

D2-D1, D2-D2, D2-D3 et D2-D4 sont désormais acquis physiquement.

La campagne D2-D couvre donc les quatre frontières transactionnelles prévues dans ce document. Cette acquisition ne qualifie pas les limites explicitement maintenues en section 8 et ne constitue pas, à elle seule, un gel documentaire D2-D.

Une anomalie de recovery arrête toute campagne complémentaire : pas de formatage, réparation ou retry avant analyse de l'état physique.
