#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameModeBase.h"
#include "Components/ActorComponent.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UObject/Interface.h"
#include "OneLifeGame.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UENUM(BlueprintType)
enum class EOneLifeNeed : uint8
{
    Hunger, Thirst, Energy, Hygiene, Social, Stress, Bladder
};

USTRUCT(BlueprintType)
struct FLifeMemory
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Id;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FText Summary;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FDateTime Date;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float EmotionalWeight = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FName,float> Consequences;
};

UINTERFACE(BlueprintType)
class UOneLifeInteractable : public UInterface
{
    GENERATED_BODY()
};

class ONELIFE_API IOneLifeInteractable
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    FText GetInteractionLabel() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    bool CanInteract(APawn* Interactor) const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void Interact(APawn* Interactor);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNeedChanged, EOneLifeNeed, Need, float, Value);

UCLASS(ClassGroup=(OneLife), meta=(BlueprintSpawnableComponent))
class ONELIFE_API UOneLifeSimulationComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOneLifeSimulationComponent();

    UPROPERTY(BlueprintAssignable) FNeedChanged OnNeedChanged;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    TMap<EOneLifeNeed,float> Needs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    TArray<FLifeMemory> Memories;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    float Money = 1250.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    float Education = 20.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    float Career = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    float Reputation = 50.f;

    UFUNCTION(BlueprintCallable) void AdvanceNeeds(float GameHours);
    UFUNCTION(BlueprintCallable) void ModifyNeed(EOneLifeNeed Need, float Delta);
    UFUNCTION(BlueprintPure) float GetNeed(EOneLifeNeed Need) const;
    UFUNCTION(BlueprintCallable) void AddMemory(const FLifeMemory& Memory);

protected:
    virtual void BeginPlay() override;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOneLifeTimeChanged, FDateTime, DateTime);

UCLASS()
class ONELIFE_API UOneLifeWorldSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable) FOneLifeTimeChanged OnTimeChanged;

    UPROPERTY(BlueprintReadOnly)
    FDateTime CurrentDateTime = FDateTime(2026,9,7,6,45,0);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RealSecondsPerGameMinute = 2.f;

    UFUNCTION(BlueprintCallable) void AdvanceMinutes(int32 Minutes);
    UFUNCTION(BlueprintPure) float GetHour() const;
};

UCLASS()
class ONELIFE_API AOneLifeCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AOneLifeCharacter();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<USpringArmComponent> CameraBoom;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UCameraComponent> Camera;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UOneLifeSimulationComponent> Simulation;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<UInputMappingContext> MappingContext;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<UInputAction> MoveAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<UInputAction> LookAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<UInputAction> JumpAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<UInputAction> SprintAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<UInputAction> InteractAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite) float WalkSpeed = 240.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float SprintSpeed = 520.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float InteractionDistance = 275.f;

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void SprintStart();
    void SprintStop();
    void Interact();
};

UCLASS()
class ONELIFE_API AOneLifeGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    AOneLifeGameMode();
};
