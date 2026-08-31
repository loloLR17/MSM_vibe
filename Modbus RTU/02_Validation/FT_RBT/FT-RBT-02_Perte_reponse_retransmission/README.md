# FT-RBT-02 — Perte de réponse et retransmission transactionnelle

## 1. Objet

FT-RBT-02 valide le comportement du moteur transactionnel lorsque la commande a été traitée côté capteur mais que la réponse n'est pas reçue par la centrale, puis que la centrale retransmet la même transaction.

## 2. Principe de propriété

FT-RBT-02 ne redéfinit pas l'idempotence du Bloc 5. Cette propriété appartient à FT-CMD-02.

FT-RBT-02 possède uniquement la composition suivante :

`transaction conforme → réponse perdue → retransmission du même transaction_id → résultat récupérable sans nouvelle exécution`.

Les oracles élémentaires sont repris sans renforcement depuis FT-CMD-02 :
- un `transaction_id` déjà traité ne doit pas être exécuté une seconde fois ;
- le résultat précédent doit être réutilisé ;
- la réponse doit rester corrélable par `cmd_active_transaction_id`.

## 3. Périmètre actif

La sous-famille couvre :
- injection contrôlée d'une perte de la première réponse après traitement de la commande ;
- retransmission immédiate de la même transaction avec le même payload ;
- récupération du résultat précédent ;
- preuve de non-réexécution uniquement si un observable discriminant existe.

## 4. Hors périmètre

Sont exclus :
- idempotence nominale sans perte de réponse : FT-CMD-02 ;
- nombre de retries : `NOT_DEFINED` ;
- délai avant retransmission : `NOT_DEFINED` ;
- backoff : `NOT_DEFINED` ;
- profondeur/durée de mémoire d'idempotence : `NOT_DEFINED` dans FT-CMD-02 ;
- même `transaction_id` avec payload différent : `NOT_DEFINED` dans FT-CMD-02 ;
- persistance après reboot : FT-PER.

## 5. Cas actifs

- `TT-RBT-B05-001` — perte de la première réponse puis retransmission du même `transaction_id` : récupération du résultat précédent (`CONDITIONAL`, injection de perte requise) ;
- `TT-RBT-B05-002` — preuve d'absence de double exécution après perte et retransmission (`CONDITIONAL`, observable discriminant requis).

## 6. Artefacts

- `source/FT-RBT-02_source.md` ;
- `detaille/FT-RBT-02_detaille.md` ;
- `detaille/FT-RBT-02_matrice_couverture.csv`.

## 7. Statut

Sous-famille reconstruite pour revue. Aucun gel ni merge sans validation explicite.
