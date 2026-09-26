# Gel firmware — P12-H3d3-D2-D — Qualification power-loss physique H3d2 / FRAM

## 1. Objet

Cette tranche gèle la qualification physique contrôlée du comportement de recovery H3d2 sur STM32U575 + FRAM réelle après pertes réelles d'alimentation à quatre frontières du protocole transactionnel.

Elle prolonge D2-C sans modifier les invariants du moteur transactionnel H3d2 ni déplacer sa sémantique dans le backend FRAM.

## 2. Baseline logicielle du gel

État de référence avant création du présent gel :

`8a0c4e3d942be6fb452f2966e62fe0bcb39e710f` — `Governance: close D2-D audit inconsistencies before freeze`

Sur cet état :

- le moteur portable `transactional_image_media.c` conserve seul la séquence transactionnelle H3d2 ;
- `stm32_fram_storage.c` reste un backend physique brut SPI/FRAM ;
- l'instrumentation D2-D reste limitée à la composition STM32 ;
- toutes les portes de qualification destructives/transactionnelles sont désarmées ;
- la validation host et la cross-compilation STM32 ont été rapportées vertes après désarmement D2-D4.

## 3. Protocole H3d2 qualifié

L'ordre transactionnel gelé et exercé physiquement reste :

1. écriture du payload complet dans l'image inactive ;
2. écriture du header final de cette image ;
3. validation complète de l'image ;
4. écriture du superbloc de publication opposé ;
5. validation de la publication ;
6. bascule d'autorité runtime après succès.

La qualification D2-D a placé les pertes réelles d'alimentation à des frontières contrôlées de cette séquence, via un adaptateur du contrat `TransactionalImagePhysicalStorage`. Le moteur portable et le format physique n'ont pas été modifiés pour fabriquer les résultats.

## 4. Résultats physiques gelés

### D2-D1 — payload partiel

Depuis `VALID / gen2 / image B / payload[0] 0xA5`, un préfixe de 4096 octets du payload de l'image inactive A a été physiquement écrit, puis une perte réelle d'alimentation a été effectuée.

Après désarmement, reprogrammation sous RESET et reboot :

`TR2_OK / VALID / gen2 / image B / payload[0] 0xA5`.

**Résultat gelé :** un payload candidat partiellement écrit et non publié n'a pas remplacé l'autorité précédente.

### D2-D2 — payload complet avant header final

Depuis la même autorité `gen2 / B / 0xA5`, les 51 818 octets du payload candidat ont été physiquement écrits avec retour `TR2_OK`. Le CPU a été immobilisé avant l'écriture du header final, puis l'alimentation a été réellement coupée.

Recovery observé :

`TR2_OK / VALID / gen2 / image B / payload[0] 0xA5`.

**Résultat gelé :** un payload candidat complet sans header final ni publication n'a pas remplacé l'autorité précédente.

### D2-D3 — image finalisée avant publication

Une première tentative D2-D3 avait un mauvais superbloc cible dans le harnais de qualification. Elle n'a subi aucune coupure et n'est pas comptée comme qualification ; elle a publié normalement `gen3 / image A / 0x5A`. Cette nouvelle autorité a été observée puis conservée comme baseline réelle de poursuite, sans restauration artificielle.

Depuis `VALID / gen3 / image A / payload[0] 0x5A`, une candidate génération 4 / image B / `0xA6` a reçu son payload complet et son header final. Le point de coupure a été atteint avant délégation de l'écriture du superbloc de publication.

Après perte réelle d'alimentation puis recovery :

`TR2_OK / VALID / gen3 / image A / payload[0] 0x5A`.

**Résultat gelé :** une image candidate physiquement complète et finalisée mais non publiée n'est pas devenue autorité.

### D2-D4 — publication physiquement écrite

Depuis `VALID / gen3 / image A / payload[0] 0x5A`, la candidate génération 4 / image B / `0xA6` a été construite et validée. Le superbloc opposé de publication a ensuite été intégralement écrit par le backend FRAM réel et cette écriture a retourné `TR2_OK`.

Le CPU a été immobilisé immédiatement après ce retour physique, avant retour au moteur H3d2, puis une perte réelle d'alimentation a été effectuée.

Après désarmement, validation complète, reprogrammation sous RESET et reboot :

`TR2_OK / VALID / gen4 / image B / payload[0] 0xA6`.

**Résultat gelé :** une publication physiquement écrite et valide a permis au recovery de sélectionner la nouvelle autorité, même si le commit n'avait pas pu retourner normalement à son appelant avant la coupure.

## 5. État physique final observé

À la clôture D2-D, l'autorité réellement récupérée en FRAM est :

- statut : `VALID` ;
- génération : `4` ;
- image active : `B` ;
- payload logique offset 0 : `0xA6`.

Cet état n'a pas été effacé ni reformatté pour le gel.

## 6. État de sécurité du firmware gelé

Les portes suivantes sont toutes désarmées dans la baseline :

- `TR2_FRAM_D2B_ALLOW_FORMAT_EMPTY = 0U` ;
- `TR2_FRAM_D2C_ALLOW_COMMIT = 0U` ;
- `TR2_FRAM_D2D1_ALLOW_PARTIAL_PAYLOAD = 0U` ;
- `TR2_FRAM_D2D2_ALLOW_COMPLETE_PAYLOAD = 0U` ;
- `TR2_FRAM_D2D3_ALLOW_FINALIZED_IMAGE = 0U` ;
- `TR2_FRAM_D2D4_ALLOW_PUBLISHED_CANDIDATE = 0U`.

Le gel ne laisse donc aucun scénario D2-D armé au boot.

## 7. Invariants conservés

Restent inchangés :

- format logique et physique H3d2 ;
- double superblock ;
- double image ;
- génération monotone ;
- validation CRC ;
- publication logique atomique ;
- recovery H3d2 ;
- absence de retry automatique après résultat physique ambigu ;
- séparation entre logique transactionnelle portable et backend physique STM32/FRAM.

## 8. Limites explicites

Ce gel qualifie les quatre frontières transactionnelles contrôlées D2-D1 à D2-D4. Il ne constitue pas une qualification exhaustive de toute forme de perte d'alimentation.

Restent explicitement hors périmètre :

- instant arbitraire de coupure à l'intérieur d'une transaction SPI individuelle ;
- timing électrique exact de la chute d'alimentation ;
- brown-out et rampes lentes d'alimentation ;
- comportement électrique du bus SPI pendant l'effondrement de tension ;
- endurance FRAM ;
- fault injection SPI/FRAM ;
- performances temporelles en exploitation.

Ces limites ne remettent pas en cause les observations physiques acquises aux frontières D2-D ; elles définissent strictement la portée du présent gel.

## 9. Conclusion

P12-H3d3-D2-D est gelable pour le périmètre suivant :

- quatre pertes réelles d'alimentation contrôlées ont été exécutées aux frontières D2-D1 à D2-D4 ;
- avant publication, le recovery a conservé l'autorité précédemment publiée ;
- après écriture physique réussie de la publication, le recovery a sélectionné la nouvelle autorité valide ;
- le moteur H3d2 portable et le backend FRAM conservent leur séparation de responsabilités ;
- les harnais destructifs/transactionnels sont désarmés dans la baseline finale.

Le détail chronologique, y compris l'incident de première tentative D2-D3, reste conservé dans `AUDIT_FIRMWARE_P12H3D3_D2D_PHYSICAL_POWERLOSS_PLAN.md`.
